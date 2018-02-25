#pragma once
#include "iServerConnection.h"
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class TpcServerConnection :
	public iServerConnection
{
public:
	TpcServerConnection();
	~TpcServerConnection();
	bool Connect(const std::string& ipAddress, const int& port) override;
	bool Disconnect() override;
	void SendData(const void* data, const int& dataSize, const PacketType& type)  override;
	void ReceiveData(void*& data, int& dataSize, PacketType& type)  override;

	void ReceiveMethod(std::future<void> futureObject)  override;
private:
	SOCKET _server;
	bool _isInitalized;
};