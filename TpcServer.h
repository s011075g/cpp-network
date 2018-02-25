#pragma once
#include "iServer.h"
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

class TpcServer 
	: public iServer
{
public:
	TpcServer();
	~TpcServer();
	bool StartServer(const int& port) override;
	bool StopServer() override;
	std::string GetUsingIP(const bool outputInformation = false) const override;

	void ClientMethod(iClient*const& client, std::future<void> futureObject) override;
protected:
	void Start(std::future<void> futureObject) override;
private:
	bool CreateListener();
private:
	bool _isInitalized;
	SOCKET _listener;
};