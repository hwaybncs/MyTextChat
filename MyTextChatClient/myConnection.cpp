#include <iostream>
#include <memory.h>
#include <cstdlib>
#include <cstdio>
#include <arpa/inet.h>
#include "myConnection.h"

using namespace std;

//tcpConnect start
tcpConnect::tcpConnect(const string &hostname, const string &servicename, bool passive)
                      : fd(0), host(hostname), service(servicename), passive(passive)
{
    if(passive)
        myListen();
    else myConnect();
}

void tcpConnect::myConnect()
{
    int sockfd, n;
    struct addrinfo hints, *res, *ressave;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if( (n = getaddrinfo(host.c_str(), service.c_str(), &hints, &res)) != 0 )
    {
        cout << "tcp connect error for " << host << " " << service << " " << gai_strerror(n) << endl;
        exit(-1);
    }
    ressave = res;

    do
    {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(sockfd < 0)
            continue;

        if(connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
            break;

        Close(sockfd);
    }while( (res = res->ai_next) != NULL);

    if(res == NULL)
    {
        cout << "tcp connect error for " << host << " " << service << endl;
        exit(-1);
    }

    fd = sockfd;
    freeaddrinfo(ressave);
}

void tcpConnect::myListen()
{
    int listenfd, n;
    const int on = 1;
    struct addrinfo hints, *res, *ressave;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if( (n = getaddrinfo(host.c_str(), service.c_str(), &hints, &res)) != 0)
    {
        cout << "tcp listen error for " << host << " " << service << " " << gai_strerror(n) << endl;
        exit(-1);
    }
    ressave = res;

    do
    {
        listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(listenfd < 0)
            continue;

        Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if(bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
            break;

        Close(listenfd);
    }while( (res = res->ai_next) != NULL);

    if(res == NULL)
    {
        cout << "tcp listen error for " << host << " " << service << endl;
        exit(-1);
    }

    listen(listenfd, 10);

    fd = listenfd;
    freeaddrinfo(ressave);
}

tcpConnection tcpConnect::myAccept() const
{
    if(passive == false)
    {
        cout << "client can not accept" << endl;
        exit(-1);
    }
    
    return tcpConnection(Accept(fd, NULL, NULL));
}

int tcpConnect::getFd() const
{
    return fd;
}
//tcpConnect end

//tcpConnection start
tcpConnection::tcpConnection(int fd) : fd(fd){}

tcpConnection::tcpConnection(tcpConnect &c)
{
    fd = c.getFd();
}

int tcpConnection::myRecvCmd(char *buff, size_t nbytes)
{
    recv(fd, inum, byteNum, 0);
    int n = atoi(inum);
    if(nbytes < n)
        exit(1);
    return recv(fd, buff, n, 0);
}

int tcpConnection::mySendCmd(const char *buff, size_t nbytes)
{
    snprintf(inum, byteNum, "%d", nbytes);
    send(fd, inum, byteNum, 0);
    return send(fd, buff, nbytes, 0);
}

int tcpConnection::myRecv(char *buff, size_t nbytes)
{
    recv(fd, inum, byteNum, 0);
    int n = atoi(inum);
    if(nbytes < n)
        exit(1);
    return recv(fd, buff, n, 0);
}

int tcpConnection::mySend(const char *buff, size_t nbytes)
{
    snprintf(inum, byteNum, "%d", nbytes);
    send(fd, inum, byteNum, 0);
    return send(fd, buff, nbytes, 0);
}
void tcpConnection::getLocal(string &host, string &service) const
{
    struct sockaddr_storage ss;
    socklen_t len;

    len = sizeof(ss);
    if(getsockname(fd, (struct sockaddr*)&ss, &len) < 0)
    {
        printf("tcpConnection::getLocal error\n");
        exit(-1);
    }

    if(ss.ss_family == AF_INET)//ipv4
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)&ss;
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr4->sin_addr, str, sizeof(str));
        host = str;

        struct servent *ser;
        ser = getservbyport(addr4->sin_port, "tcp");
        service = ser->s_name;
    }
    else if(ss.ss_family == AF_INET6)//ipv6
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)&ss;
        char str[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &addr6->sin6_addr, str, sizeof(str));
        host = str;

        struct servent *ser;
        ser = getservbyport(addr6->sin6_port, "tcp");
        service = ser->s_name;
    }
}

void tcpConnection::getOther(string &host, string &service) const
{
    struct sockaddr_storage ss;
    socklen_t len;

    len = sizeof(ss);
    if(getpeername(fd, (struct sockaddr*)&ss, &len) < 0)
    {
        printf("tcpConnection::getOther error\n");
        exit(-1);
    }

    if(ss.ss_family == AF_INET)//ipv4
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)&ss;
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr4->sin_addr, str, sizeof(str));
        host = str;

        struct servent *ser;
        ser = getservbyport(addr4->sin_port, "tcp");
        service = ser->s_name;
    }
    else if(ss.ss_family == AF_INET6)//ipv6
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)&ss;
        char str[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &addr6->sin6_addr, str, sizeof(str));
        host = str;

        struct servent *ser;
        ser = getservbyport(addr6->sin6_port, "tcp");
        service = ser->s_name;
    }
}
//tcpConnection end

//udpConnection start

//udpConnection end
