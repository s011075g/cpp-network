#include "TpcServer.h"
#include "TpcClient.h"
#include <iostream>

TpcServer::TpcServer()
	: iServer(), _listener(0)
{
	//Initalize WinSock
	WSADATA wsData;
	const WORD ver = MAKEWORD(2, 2);
	const int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "Cannot Initalize WinSock!" << std::endl;
		_isInitalized = false;
	}
	else
		_isInitalized = true;
}

TpcServer::~TpcServer()
{
	if (_isServerRunning)
		this->TpcServer::StopServer(); //calls the correct pure virtual function
	WSACleanup();
}

bool TpcServer::StartServer(const int& port)
{
	if (!_isInitalized)
		return false;
	if (_isServerRunning)
		StopServer();
	_port = port;

	_listeningThread = new std::thread(&TpcServer::Start, this, _exitSignal.get_future());
	
	return _listeningThread->joinable();
}

bool TpcServer::StopServer()
{
	// TODO: tell clients to disconnect
	if (!_isInitalized)
		return true;
	if (!_isServerRunning)
		return true;
	_isServerRunning = false;
	_exitSignal.set_value();
	if (_listeningThread->joinable())
	{
		closesocket(_listener);
		_listeningThread->join();
		return true;
	}
	return false;
}

std::string TpcServer::GetUsingIP(const bool outputInformation) const
{
	std::string result = "";
	char hostName[NI_MAXHOST] = { 0 };
	if(gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR)
	{
		std::cerr << "Error getting local host name" << std::endl;
		return nullptr;
	}

	addrinfo hints = { 0 };
	hints.ai_family = AF_INET;    // Want IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo *addrInfo(nullptr);

	if (getaddrinfo(hostName, nullptr, &hints, &addrInfo) != 0)
	{
		std::cerr << "Error getting address information!" << std::endl;
		return nullptr;
	}
	for (auto ptr = addrInfo; ptr != nullptr; ptr = ptr->ai_next)
	{
		char ipAddress[NI_MAXHOST] = {0};
		if (ptr->ai_family == AF_INET)
		{
			if (getnameinfo(ptr->ai_addr, sizeof(struct sockaddr_in), ipAddress, _countof(ipAddress) - 1, nullptr, 0, NI_NUMERICHOST) == 0)
				result = std::string(ipAddress);
		}
	}
	freeaddrinfo(addrInfo);

	if (outputInformation)
		std::cout << hostName << " is hosting server on " << result.c_str() << ", port " << _port << std::endl;
	return result;
}

void TpcServer::ClientMethod(iClient*const& client, std::future<void> futureObject)
{
	while (futureObject.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout)
	{
		void* data (nullptr);
		int dataSize(0);
		PacketType type(P_NONE);
		client->ReceiveData(data, dataSize, type);
		std::cout << (char*)data << std::endl;
		_clientListAccess.lock();
		for(auto c : _clients)
			if(c != client)
				c->SendData(data, dataSize + 1, type);
		_clientListAccess.unlock();
		free(data);
	}
}

void TpcServer::Start(std::future<void> futureObject)
{
	std::cout << "Server Started\n";
	_isServerRunning = true;
	try
	{
		if(CreateListener())
			while(futureObject.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout)
			{
				sockaddr_in client;
				int clientSize(sizeof(client));
				SOCKET clientSocket = accept(_listener, reinterpret_cast<sockaddr*>(&client), &clientSize);
				if(clientSocket == 4294967295)
				{
					std::cerr << "Server Stopping!" << std::endl;
					break;
				}
				if (clientSocket == INVALID_SOCKET)
				{
					std::cerr << "Invalid Client socket! #" << clientSocket << std::endl;
					StopServer();
					break;
				}

				char host[NI_MAXHOST];		//Client's remote name
				char service[NI_MAXSERV];	//Service (port) the client is connected to

				ZeroMemory(host, NI_MAXHOST);
				ZeroMemory(service, NI_MAXSERV);

				if (getnameinfo(reinterpret_cast<sockaddr*>(&client), clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
				{
					std::cout << host << " connected on port " << service << std::endl;
				}
				else
				{
					inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
					std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
				}

				TpcClient * tpcClient = new TpcClient(this, clientSocket);
				_clientListAccess.lock();
				_clients.push_back(tpcClient);
				_clientListAccess.unlock();
				tpcClient->Start();
			}
	}
	catch(std::exception e)
	{
		std::cerr << e.what() << std::endl;
		StopServer();
	}
}

bool TpcServer::CreateListener()
{
	//Create listening Socket
	_listener = socket(AF_INET, SOCK_STREAM, 0); //SOCK_DGRAM is another type
	if (_listener == INVALID_SOCKET)
	{
		std::cerr << "Cannot create Socket!" << std::endl;
		return false;
	}
	//Bind ip and port address to the Socket
	sockaddr_in hint;
	hint.sin_family = AF_INET; //IPv4
	hint.sin_port = htons(_port);//host to network short - htons
	hint.sin_addr.S_un.S_addr = INADDR_ANY; 

	bind(_listener, reinterpret_cast<sockaddr*>(&hint), sizeof(hint)); //Binding

	//winsock socket is set to listen
	listen(_listener, SOMAXCONN);
	return true;
}