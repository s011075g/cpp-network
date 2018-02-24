#pragma once
#include "iClient.h"
#include <WS2tcpip.h>
class TpcClient :
	public iClient
{
public:
	TpcClient(iServer*const& server, const SOCKET& clientSocket);
	~TpcClient();
	void Start() override;
	void ReceiveData(void*& data, int& dataSize, PacketType& type) override;
	void SendData(const void* data, const int& dataSize, const PacketType& type) override;
	void CloseClient() override;
private:
	SOCKET _clientSocket;
};