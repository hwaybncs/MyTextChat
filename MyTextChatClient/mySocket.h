#ifndef _MYSOCKET_H_
#define _MYSOCKET_H_

#include <sys/socket.h>

int Socket(int family, int type, int protocol);
void Connect(int fd, const struct sockaddr *sa, socklen_t salen); 
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Listen(int fd, int backlog);
int Accept(int fd, struct sockaddr *sa, socklen_t *salen);

void Close(int fd);
void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);

#endif
