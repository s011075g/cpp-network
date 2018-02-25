#include <iostream>
#include "TpcServer.h"
#include <string>

int main()
{
	iServer* server = new TpcServer();
	if (!server->StartServer(43081))
	{
		delete server;
		return -1;
	}
	std::string input = "";
	while(input.compare("stop") != 0)
	{
		std::cout << "> ";
		std::getline(std::cin, input);
		if (input.compare("info") == 0)
			server->GetUsingIP(true);
	}
	server->StopServer();
	delete server;
	return 1;
}
