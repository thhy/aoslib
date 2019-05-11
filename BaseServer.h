#pragma once
#include <string>
class BaseServer
{
public:
	BaseServer() = delete;
	BaseServer(std::string ip, unsigned int port) :mIp(ip), mPort(port) {

	};
	virtual ~BaseServer();

private:
	std::string mIp;
	unsigned int mPort;
	unsigned int epoll_t;
};

