#pragma once
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <boost/noncopyable.hpp>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <iostream>
#include <list>

#include <string.h>
#include "eventLoop.h"

class EventLoop;

class Socket:public boost::noncopyable
{
public:
	Socket(std::string ip = "0.0.0.0", unsigned int port = 8080, EventLoop *manage = nullptr);

	Socket(int sockfd, EventLoop *manage = nullptr);

	void server();

	int acceptCli();

	virtual std::string readMsg();

	virtual void writeMsg(std::string msg, bool flag = false);

	int getFd();

	int setEvent(int ev = EPOLLIN);

	int getEvent();

	std::string getLastMsg();

	virtual ~Socket();

private:
	std::string mIp;
	unsigned int mPort;
	unsigned int fd;
	sockaddr_in listenAddr;
	std::string errMsg;
	int event;
	EventLoop *loop;
	std::list <std::string> buffList;
};

