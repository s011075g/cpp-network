#pragma once
#include <thread>
#include <future>
#include <mutex>
#include <string>
#include "../Packets.h"

class iServerConnection
{
protected:
	std::string _ipAddress;
	int _port;
	std::thread* _receiveDataThread;
	std::promise<void> _exitSignal;
	std::mutex _consoleAccess;
	bool _isConnected;
public:
	iServerConnection() : _ipAddress(""), _port(0), _receiveDataThread(nullptr), _isConnected(false) {};
	virtual ~iServerConnection() {};
	virtual bool Connect(const std::string& ipAddress, const int& port) = 0;
	virtual bool Disconnect() = 0;
	virtual void SendData(const void* data, const int& dataSize, const PacketType& type) = 0;
	virtual void ReceiveData(void*& data, int& dataSize, PacketType& type) = 0;
	virtual bool IsConnected() const { return _isConnected; }
	
	virtual void ReceiveMethod(std::future<void> futureObject) = 0;
};