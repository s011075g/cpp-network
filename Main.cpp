/*#include <iostream>
#include <thread>
#include <string>

void Thread(const std::string name)
{
	for (auto i = 1; i <= 10; i++)
		std::cout << name << ": " << i << std::endl;
}

int main()
{
	std::thread thread(Thread, "Thread 1");
	std::thread thread2(Thread, "Thread 2");
	if (thread.joinable())
		thread.join();
	if (thread2.joinable())
		thread2.join();
	std::cin.get();
	return 0;
}*/

#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

int main()
{
	//Initalize winsock
	WSADATA wsData;
	const WORD ver = MAKEWORD(2, 2);
	const int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "Cannot Initalize WinSock!" << std::endl;
		return -1;
	}
	//Create a socket
	const SOCKET listener = socket(AF_INET, SOCK_STREAM, 0); //SOCK_DGRAM
	if(listener == INVALID_SOCKET)
	{
		std::cerr << "Cannot create Socket!" << std::endl;
		WSACleanup();
		return -1;
	}
	//Bind the ip and port address to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(27763);//host to network short - htons
	hint.sin_addr.S_un.S_addr = INADDR_ANY; //Could also use inet_pton

	bind(listener, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));

	//winsock socket is listening
	listen(listener, SOMAXCONN);

	//wait for connection
	sockaddr_in client;
	int clientSize = sizeof(client);
	const SOCKET clientSocket = accept(listener, reinterpret_cast<sockaddr*>(&client), &clientSize);
	if(clientSocket == INVALID_SOCKET)
	{
		std::cerr << "Invalid Client socket!" << std::endl;
		WSACleanup();
		return -1;
	}

	char host[NI_MAXHOST];		//Client's remote name
	char service[NI_MAXSERV];	//Service (port) the client is connected to

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if(getnameinfo(reinterpret_cast<sockaddr*>(&client), clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
	}

	//close listening
	closesocket(listener);

	//whileloop: accept and echo message
#define bufferSize 4096
	char buf[bufferSize];
	while(true)
	{
		ZeroMemory(buf, bufferSize);
		//wait for client to send data
		const int byteReceived = recv(clientSocket, buf, bufferSize, 0);
		if (byteReceived == SOCKET_ERROR)
		{
			std::cerr << "Error in recv()!" << std::endl;
			break;
		}
		if (byteReceived == 0)
		{
			std::cout << "Client disconnected" << std::endl;
			break;
		}

		//echo message back to client
		send(clientSocket, buf, byteReceived + 1, 0);

	}

	//close socket
	closesocket(clientSocket);

	//Cleanup winsock
	WSACleanup();
}