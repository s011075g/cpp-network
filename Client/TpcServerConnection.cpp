#include "TpcServerConnection.h"
#include <iostream>

TpcServerConnection::TpcServerConnection()
	: iServerConnection()
{
	//Initalize WinSock
	WSADATA wsData;
	const WORD ver = MAKEWORD(2, 2);
	const int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "Cannot Initalize WinSock! #" << wsOk << std::endl;
		_isInitalized = false;
	}
	else
		_isInitalized = true;
}

TpcServerConnection::~TpcServerConnection()
{
	if (_isConnected)
		this->TpcServerConnection::Disconnect(); //ensures it calls the correct pure virtual method
	WSACleanup();
}

bool TpcServerConnection::Connect(const std::string& ipAddress, const int& port)
{
	if (!_isInitalized)
		return false;
	if (_isConnected)
		Disconnect();
	_ipAddress = ipAddress;
	_port = port;
	//Create Socket
	_server = socket(AF_INET, SOCK_STREAM, 0);
	if(_server == INVALID_SOCKET)
	{
		std::cerr << "Cannot create Socket! #" << WSAGetLastError() << std::endl;
		return false;
	}
	//Fill in a hint
	sockaddr_in hint; //_in makes it ipv4
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);//host to network short - htons
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr); //Telling the hint what server wer want to connect to

	//Connect to server
	const int connectionResult = connect(_server, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));
	if (connectionResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server! #"<< connectionResult << std::endl;
		closesocket(_server);
		return false;
	}

	_receiveDataThread = new std::thread(&TpcServerConnection::ReceiveMethod, this, _exitSignal.get_future());
	
	return _isConnected = true;
}

bool TpcServerConnection::Disconnect()
{
	if (!_isConnected)
		return true;
	_isConnected = false;
	_exitSignal.set_value();
	if (_receiveDataThread->joinable())
	{
		closesocket(_server);
		_receiveDataThread->join();
		return true;
	}
	return false;
}

void TpcServerConnection::SendData(const void* data, const int& dataSize, const PacketType& type)
{
	int size = dataSize + 1;
	send(_server, reinterpret_cast<const char*>(&size), sizeof(int), 0);
	send(_server, static_cast<const char*>(data), size, 0);
	send(_server, reinterpret_cast<const char*>(&type), sizeof(type), 0);
}

void TpcServerConnection::ReceiveData(void*& data, int& dataSize, PacketType& type)
{
	recv(_server, reinterpret_cast<char*>(&dataSize), sizeof(int), 0);
	char* buffer = new char[dataSize] {0};
	data = buffer;
	recv(_server, static_cast<char*>(data), dataSize, 0);
	recv(_server, reinterpret_cast<char*>(&type), sizeof(type), 0);
}

void TpcServerConnection::ReceiveMethod(std::future<void> futureObject)
{
	while (futureObject.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout)
	{
		void* data(nullptr);
		int dataSize(0);
		PacketType type(P_NONE);
		ReceiveData(data, dataSize, type);
		_consoleAccess.lock();
		std::string result((char*)data);
		std::cout << result << std::endl;
		_consoleAccess.unlock();
		delete[] (char*)data;
	}
}