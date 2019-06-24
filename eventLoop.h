#pragma once
#include <map>
#include "common.h"
#include "SingletonImp.h"
#include "BaseServer.h"

class Socket;

enum Event
{
    AOSREAD = 1,
    AOSWRITE = 2,
    AOSLISTEN = 4,
    AOSDELETE = 8
};

class EventLoop /*: public SingletonImp<EventLoop>*/
{
public:
    static const int MAXEVENTS = 1024;
    EventLoop();
    void addReadEvent(Socket *skt);
    void run();
	int modEvent(Socket *skt);
private:
    
    int epoll_t;
	int listenFd;
};
