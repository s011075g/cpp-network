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

	void ClientMethod(iClient*const& client, std::future<void> futureObject) override;
protected:
	void Start(std::future<void> futureObject) override;
private:
	void CreateListener();
private:
	bool _isInitalized;
	SOCKET _listener;
};

//Init winsock
//shut down winsock
//create socket
//close socket
//bind socket to an ip address and port
//winsock socket is listening
//wait for connection
//close listening
//whileloop: accept and echo message
