#include "eventLoop.h"
#include <sys/fcntl.h>
#include "aosLogger.h"

EventLoop::EventLoop()
{
    epoll_t = epoll_create(1024);
    assert(epoll_t > 0);
}

void EventLoop::addReadEvent(Socket *skt)
{
    epoll_event event;
    if (skt->getEvent() & AOSREAD)
        event.events |= EPOLLIN;
    if(skt->getEvent() & AOSWRITE)
        event.events |= EPOLLOUT;
    event.data.fd = skt->getFd();
    event.data.ptr = skt;
    std::cout << "add fd:" << skt->getFd() <<std::endl;
    epoll_ctl(epoll_t, EPOLL_CTL_ADD, skt->getFd(), &event);
    // fcntl(epoll_t, F_SETFD, fcntl(epoll_t, F_GETFD)|O_NONBLOCK);
}

void EventLoop::run()
{
    epoll_event events[MAXEVENTS];
    auto ret = epoll_wait(epoll_t, events, MAXEVENTS, -1);
    std::cout << "recv fd count:" << ret << std::endl;
    if (ret == -1)
    {
        LOGDEBUG << strerror(errno);
    }
    for(int i = 0; i < ret; ++i)
    {
        Socket *skt = (Socket *)events[i].data.ptr;
        std::cout << "event:" << skt->getEvent() << std::endl;
        if(skt->getEvent() & AOSLISTEN)
        {
            int cliFd = skt->acceptCli();
            if (cliFd == -1)
            {
                std::cout << strerror(errno)<< std::endl;
                continue;
            }
            Socket *cliSock = new Socket(cliFd, this);
            this->addReadEvent(cliSock);
            cliSock->setEvent(AOSREAD);
            std::cout << "add:" << cliFd << std::endl;
        }
        else if (skt->getEvent() & AOSREAD)
        {
            std::cout << "read:" << std::endl;
            std::cout << skt->readMsg() << std::endl;
        }
        else if (skt->getEvent() & AOSWRITE)
        {
            skt->writeMsg(skt->getLastMsg());
        }
    }
}

void EventLoop::modEvent(Socket *skt)
{
    if (skt->getEvent() & AOSDELETE)
    {
        epoll_ctl(epoll_t, EPOLL_CTL_DEL, skt->getFd(), NULL);
    }
    else
    {
        epoll_event ev;
        if (skt->getEvent() & AOSREAD)
        {
            ev.events |= EPOLLIN;
        }
        if (skt->getEvent() & AOSWRITE)
        {
            ev.events |= EPOLLOUT;
        }
        ev.data.fd = skt->getFd();
        ev.data.ptr = skt;
        epoll_ctl(epoll_t, EPOLL_CTL_MOD, skt->getFd(), &ev);
    }
}