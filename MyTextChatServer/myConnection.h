#ifndef _MYCONNECTION_H_
#define _MYCONNECTION_H_

#include <string>
#include <netdb.h>
#include "mySocket.h"

class tcpConnect;

class tcpConnection
{
public:
    tcpConnection(tcpConnect &c);//根据fd得到相应的host和service
    tcpConnection(int fd = -1);
    int getFd() const {return fd;}
    void setFd(int f) {fd = f;}
    int myRecvCmd(char *buff, size_t nbytes);
    int mySendCmd(const char *buff, size_t nbytes);
    int myRecv(char *buff, size_t nbytes);
    int mySend(const char *buff, size_t nbytes);
    void getLocal(std::string &host, std::string &service) const;
    void getOther(std::string &host, std::string &service) const;
    void close(){ if(fd != -1) Close(fd), fd = -1;}

private:
    int fd;
    static const int byteNum = 6;
    char inum[byteNum];
    //std::string host;
    //std::string service;
};

class tcpConnect
{
public:
    tcpConnect(const std::string &hostname, const std::string &servicename, bool passive);
    tcpConnection myAccept() const;//返回fd
    int getFd() const;
    void close(){ if(fd != -1) Close(fd), fd = -1;}

private:
    void myConnect();
    void myListen();

    int fd;
    std::string host;
    std::string service;
    //struct sockaddr *sa;
    //socklen_t salen;
    bool passive;
};

/*
class udpConnection
{
public:
    udpConnection(const string &hostname, const string &servicename);
    int recv(string &str);
    int send(const string &str);

private:
    int client();
    int connect();
    int listen();

    int fd;
    string host;
    string service;
    struct sockaddr *sa;
    socklen_t salen;
    bool passive;
};
*/

#endif
