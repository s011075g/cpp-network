#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include "../Packets.h"
#include "TpcServerConnection.h"
#pragma comment(lib, "ws2_32.lib")
int main()
{
	/*const std::string ipAddress = "127.0.0.1";
	const int port = 43081;

	//Initalize WinSock
	WSADATA wsData;
	const WORD ver = MAKEWORD(2, 2);
	const int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "Cannot Initalize WinSock!" << std::endl;
		return -1;
	}

	//Create Socket
	SOCKET listener = socket(AF_INET, SOCK_STREAM, 0); //SOCK_DGRAM
	if (listener == INVALID_SOCKET)
	{
		std::cerr << "Cannot create Socket!" << std::endl; //WSAGetLastError()
		WSACleanup();
		return -1;
	}

	//Fill in a hint
	sockaddr_in hint; //_in makes it ipv4
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);//host to network short - htons
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);
	//hint.sin_addr.S_un.S_addr = INADDR_ANY;

	//Connect to server
	const int connectionResult = connect(listener, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));
	if (connectionResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server!" << std::endl;
		closesocket(listener);
		WSACleanup();
		return -1;
	}
	//While loop*/
/*#define bufferSize 4096
	char buf[bufferSize];
	std::string userInput;
	do
	{
		//Prompt for input
		std::cout << "CLient> ";
		std::getline(std::cin, userInput);
		if (userInput.size() > 0) //Checking for input
		{
			//Send text
			int size = userInput.size();
			const int send1 = send(listener, (char*)&size, sizeof(int), 0);
			const int sendResult = send(listener, userInput.c_str(), userInput.size() + 1, 0);
			PacketType type = P_NONE;
			const int send2 = send(listener,(char*)&type, sizeof(type),0);
			if (sendResult != SOCKET_ERROR)
			{
				//wait for response
				ZeroMemory(buf, bufferSize);
				int bytesReceived = recv(listener, buf, bufferSize, 0);
				if (bytesReceived > 0)
				{//Echo response to console
					std::cout << "Server> " << std::string(buf, 0, bytesReceived) << std::endl;
				}
			}
		}
	} while (userInput.size() > 0);
	//Clean up

	closesocket(listener);

	WSACleanup();

	return 0;*/
	std::string ipAddress = "";
	int port = 0;
	std::cout << "Server IPv4 Address: ";
	std::cin >> ipAddress;
	std::cout << "Server Port: ";
	std::cin >> port;
	iServerConnection* connection = new TpcServerConnection();
	if (!connection->Connect(ipAddress, port))
	{
		delete connection;
		return -1;
	}
	std::string input = "";
	while(input.compare("disconnect") != 0)
	{
		std::cout << "> ";
		std::getline(std::cin, input);
		if(input.compare("disconnect") != 0 && input.size() > 0)
		{
			connection->SendData((void*)input.c_str(), input.size(), P_NONE);
		}
	}
	connection->Disconnect();
	delete connection;
	return 1;
}