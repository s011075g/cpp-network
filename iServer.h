#pragma once
#include <Thread>
#include <future>
#include <mutex>
#include <list>
#include "iClient.h"
class iServer
{
protected:
	int _port;
	std::thread* _listeningThread;
	std::promise<void> _exitSignal;
	std::mutex _clientListAccess;
	std::list<iClient*> _clients;
	bool _isServerRunning;
public:
	iServer() : _port(0), _listeningThread(nullptr), _isServerRunning(false) { ; };
	virtual ~iServer()
	{
		for (auto c : _clients)
		{
			if (c->IsOpen())
				c->CloseClient();
			delete c;
		}
		_clients.clear();
	};
	virtual bool StartServer(const int& port) = 0;
	virtual bool StopServer() = 0;

	virtual void ClientMethod(iClient*const& client, std::future<void> futureObject) = 0;
protected:
	virtual void Start(std::future<void> futureObject) = 0;
};
