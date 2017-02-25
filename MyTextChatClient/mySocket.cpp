#include <iostream>
#include <cstdlib>
#include "mySocket.h"

int Socket(int family, int type, int protocol)
{
    int n;
    if( (n = socket(family, type, protocol)) < 0)
    {
        std::cout << "socket error" << std::endl;
        exit(-1);
    }
    return n;
}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if(connect(fd, sa, salen) == -1)
    {
        std::cout << "connect error" << std::endl;
        exit(-1);
    }
}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if(bind(fd, sa, salen) == -1)
    {
        std::cout << "bind error" << std::endl;
        exit(-1);
    }
}
void Listen(int fd, int backlog)
{
    if(listen(fd, backlog) == -1)
    {
        std::cout << "listen error" << std::endl;
        exit(-1);
    }
}
int Accept(int fd, struct sockaddr *sa, socklen_t *salen)
{
    int n = accept(fd, sa, salen);
    if(n < 0)
    {
        std::cout << "accept error" << std::endl;
        exit(-1);
    }
    return n;
}

void Close(int fd)
{
    if(close(fd) == -1)
    {
        std::cout << "close error" << std::endl;
        exit(-1);
    }
}

void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
    if(setsockopt(fd, level, optname, optval, optlen) < 0)
    {
        std::cout << "setsockopt error" << std::endl;
        exit(-1);
    }
}
