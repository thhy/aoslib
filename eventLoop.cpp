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
    LOGDEBUG << "add fd:" << skt->getFd() <<std::endl;
    if (epoll_ctl(epoll_t, EPOLL_CTL_ADD, skt->getFd(), &event) == -1)
	{
		LOGDEBUG << strerror(errno) << " add fd " << skt->getFd() << "failed";
		return;

	}
	
    // fcntl(epoll_t, F_SETFD, fcntl(epoll_t, F_GETFD)|O_NONBLOCK);
}

void EventLoop::run()
{
    epoll_event events[MAXEVENTS];

    auto ret = epoll_wait(epoll_t, events, MAXEVENTS, -1);
    LOGDEBUG << "recv fd count:" << ret;
    if (ret == -1)
    {
        LOGDEBUG << strerror(errno);
    }
    for(int i = 0; i < ret; ++i)
    {
        Socket *skt = (Socket *)events[i].data.ptr;
        LOGDEBUG << "fd:"<<skt->getFd() << "event:" << skt->getEvent() << std::endl;
		if((events[i].events & EPOLLERR)||(events[i].events & EPOLLHUP))
        {
            /* An error has occured on this fd, or the socket is not
               ready for reading (why were we notified then?) */
            LOGERROR << "epoll error";;
            close(events[i].data.fd);
			delete skt;
            continue;
        }

        if(skt->getEvent() & AOSLISTEN)
        {
            int cliFd = skt->acceptCli();
            if (cliFd == -1)
            {
                LOGERROR << strerror(errno)<< std::endl;
                continue;
            }
            Socket *cliSock = new Socket(cliFd, this);
            this->addReadEvent(cliSock);
            cliSock->setEvent(AOSREAD);
            LOGDEBUG << "add fd:" << cliFd;
        }
        else if (skt->getEvent() & AOSREAD)
        {
            LOGDEBUG << "read:" << skt->readMsg();
        }
        else if (skt->getEvent() & AOSWRITE)
        {
            skt->writeMsg(skt->getLastMsg());
        }
		if (skt->getEvent() & AOSDELETE)
		{
			LOGDEBUG << "delete fd:" << skt->getFd();
			delete skt;
		}
    }
}

int EventLoop::modEvent(Socket *skt)
{
    if (skt->getEvent() & AOSDELETE)
    {
        LOGDEBUG << "close fd:" << skt->getFd();
        int tmperrno = errno;
        errno = 0;
        if (epoll_ctl(epoll_t, EPOLL_CTL_DEL, skt->getFd(), NULL) == -1)
        {
        	errno = tmperrno;
            LOGDEBUG << strerror(errno);
			return -1;
        }
		close(skt->getFd());
		//delete skt;
        errno = tmperrno;
    }
    else
    {
        epoll_event ev;
        if (skt->getEvent() & AOSREAD)
        {
            ev.events |= EPOLLIN |EPOLLET;
        }
        if (skt->getEvent() & AOSWRITE)
        {
            ev.events |= EPOLLOUT |EPOLLET;
        }
        ev.data.fd = skt->getFd();
        ev.data.ptr = skt;
        if (epoll_ctl(epoll_t, EPOLL_CTL_MOD, skt->getFd(), &ev) == -1)
        {
            LOGERROR << strerror(errno);
			return -1;
        }
    }
	return 0;
}
