#pragma once
#include <Thread>
#include <future>
#include "Packets.h"

class iServer;

class iClient
{
protected:
	std::thread* _clientThread;
	std::promise<void> _exitSignal;
	iServer*const& _server;
	bool _isOpen;
public:
	explicit iClient(iServer*const& server) : _clientThread(nullptr), _server(server), _isOpen(false) { ; };
	virtual ~iClient() { delete _clientThread;};
	virtual void Start() = 0;
	virtual void ReceiveData(void*& data, int& dataSize, PacketType& type) = 0;
	virtual void SendData(const void* data, const int& dataSiz, const PacketType& type) = 0;
	virtual void CloseClient() = 0;
	virtual bool IsOpen() const { return _isOpen; } 
};
