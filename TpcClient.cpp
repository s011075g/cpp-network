#include "TpcClient.h"
#include "iServer.h"

TpcClient::TpcClient(iServer*const& server, const SOCKET& clientSocket)
	: iClient(server), _clientSocket(clientSocket)
{ 
	_isOpen = true;
}

TpcClient::~TpcClient()
{ }

void TpcClient::Start()
{
	_clientThread = new std::thread(&iServer::ClientMethod, _server, this, _exitSignal.get_future());
}

void TpcClient::ReceiveData(void*& data, int& dataSize, PacketType& type)
{
	recv(_clientSocket, reinterpret_cast<char*>(&dataSize), sizeof(int), 0);
	data = malloc(dataSize);
	recv(_clientSocket, static_cast<char*>(data), dataSize, 0);
	recv(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), 0);
}

void TpcClient::SendData(const void* data, const int& dataSize, const PacketType& type)
{
	send(_clientSocket, reinterpret_cast<const char*>(dataSize), sizeof(int), 0);
	send(_clientSocket, static_cast<const char*>(data), dataSize, 0);
	send(_clientSocket, reinterpret_cast<const char*>(type), sizeof(type), 0);
}

void TpcClient::CloseClient()
{
	_exitSignal.set_value();

	if (_clientThread->joinable())
	{
		_clientThread->join();
		closesocket(_clientSocket);
	}
	_isOpen = false;
}