#pragma once

#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

class Server
{
public:
	Server();
	~Server();
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
