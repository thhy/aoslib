#pragma once
#include <map>
#include "common.h"
#include "SingletonImp.h"
#include "BaseServer.h"

class Socket;

const int INVALIDSOCKET = -1;
const int AOSEPOLLIN = EPOLLIN | EPOLLERR | EPOLLHUP;
const int AOSEPOLLOUT = EPOLLOUT | EPOLLERR | EPOLLHUP;
const int AOSEPOLLET = EPOLLET;
// const int AOSEPOLLLT = EPOLLLT

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
    void onAccept();
    void onRead();
    void onWrite();
private:
    
    int epoll_t;
	int listenFd;
};
