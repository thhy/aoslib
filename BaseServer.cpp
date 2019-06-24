#include "BaseServer.h"
#include "aosLogger.h"
int setNonBlocking(int fd)
{
	if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0)|O_NONBLOCK)==-1)
	{
		LOGERROR << strerror(errno);
		return -1;
	}
	return 0;
}

Socket::Socket(std::string ip , unsigned int port, EventLoop *manage) : mIp(ip), mPort(port), loop(manage)
{

    bzero(&listenAddr, sizeof(sockaddr_in));
    in_addr digIp;
    inet_pton(AF_INET, mIp.c_str(), &digIp);
    listenAddr.sin_addr = digIp;
    listenAddr.sin_port = htons(mPort);
    LOGDEBUG << "port:" << mPort << std::endl;
    LOGDEBUG << "htonl port:" << htonl(mPort) << std::endl;
};

Socket::Socket(int sockfd, EventLoop *manage) : fd(sockfd), loop(manage)
{
    socklen_t len = sizeof(listenAddr);
    if (!getpeername(sockfd, (struct sockaddr *)&listenAddr, &len))
    {
        LOGDEBUG << "对方IP：%s " << inet_ntoa(listenAddr.sin_addr);
        LOGDEBUG << "对方PORT：%d " << ntohs(listenAddr.sin_port) << std::endl;
    }
    const char chOpt = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&chOpt, sizeof(char));
    setsockopt(fd, IPPROTO_TCP, SO_LINGER, (void *)&chOpt, sizeof(char));
    setsockopt(fd, IPPROTO_TCP, SOCK_NONBLOCK, (void *)&chOpt, sizeof(char));
    setsockopt(fd, IPPROTO_TCP, SO_REUSEADDR, (void *)&chOpt, sizeof(char));
	setNonBlocking(fd);
}

void Socket::server()
{
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    const char chOpt = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&chOpt, sizeof(char));
    setsockopt(fd, IPPROTO_TCP, SO_LINGER, (void *)&chOpt, sizeof(char));
    setsockopt(fd, IPPROTO_TCP, SOCK_NONBLOCK, (void *)&chOpt, sizeof(char));
    setsockopt(fd, IPPROTO_TCP, SO_REUSEADDR, (void *)&chOpt, sizeof(char));
    assert(fd > 0);
    int ret = bind(fd, (sockaddr *)&listenAddr, sizeof(sockaddr));
    char buff[1024];
    inet_ntop(AF_INET, &listenAddr.sin_addr.s_addr, buff, sizeof(buff));
    LOGDEBUG << "bind to " << buff << ":" << ntohs(listenAddr.sin_port) << std::endl;
    assert(ret != -1);

	setNonBlocking(fd);
    ret = listen(fd, 100);
    assert(ret != -1);
}

int Socket::acceptCli()
{
    sockaddr_in cliAddr;
    socklen_t cliAddrLen = sizeof(sockaddr_in);
    int cliFd = -1;
    while ((cliFd = accept(fd, (sockaddr *)&cliAddr, &cliAddrLen)) != -1)
    {
        char buff[1024];
        inet_ntop(AF_INET, &cliAddr.sin_addr.s_addr, buff, sizeof(buff));
        LOGDEBUG << fd << " accept " << buff << ":" << cliAddr.sin_port << std::endl;
        if (cliFd > 0)
        {
            return cliFd;
        }
    }
    errMsg = strerror(errno);
    return cliFd;
}

std::string Socket::readMsg()
{
    std::string mBuff;
    int len = 0;
    int ev = this->event & (~AOSREAD) | AOSWRITE;
    while (true)
    {
        char buff[2048];
		LOGDEBUG << "waiting read...";
        int ret = read(fd, buff, sizeof(buff));
        if (ret > 0)
        {

            mBuff.append(buff, ret);
            len += ret;
            if (ret < sizeof buff)
            {
                break;
            }
            LOGDEBUG << "read size:" << ret;
        }
        else if (ret < 0)
        {
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            else
            {
                LOGDEBUG << strerror(errno) << std::endl;
                break;
            }
        }
        else
        {
            //ret == 0
            if (len == 0)
            {
                //should close fd
                LOGDEBUG << "close fd:" << fd << std::endl;
                ev = this->event | AOSDELETE;
                //close(fd);
                break;
            }
            else
            {
                break;
            }
        }
    }
    this->setEvent(ev);
    this->buffList.push_back(mBuff);
    return mBuff;
}

void Socket::writeMsg(std::string msg, bool flag)
{
    int ev = this->event & (~AOSWRITE) | AOSREAD;
    write(fd, msg.c_str(), msg.size());
    if (flag)
    {
    }
    this->setEvent(ev);
}

int Socket::getFd()
{
    return fd;
}

int Socket::setEvent(int ev)
{
    event = ev;
    return this->loop->modEvent(this);
}

int Socket::getEvent()
{
    return event;
}

Socket::~Socket()
{
    if (fd > 0)
    {
        close(fd);
    }
};

std::string Socket::getLastMsg()
{
    std::string msg = this->buffList.front();
    this->buffList.pop_front();
    return msg;
}
