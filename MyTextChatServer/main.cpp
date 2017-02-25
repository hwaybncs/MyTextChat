#include <list>
//#include <utility>
#include <pthread.h>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include "myConnection.h"
#include "myTextChat.h"

using namespace std;

long accountBase;
long accountIncre;
list<pair<char*, bool> > accountList;//password, bool(accountBase+index+1 is account)
list<long> accountList2;

pthread_mutex_t accountMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t dataMutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    mkdir("data", 00775);
    void *work(void*);

    tcpConnect listenConn("192.168.159.130", "6666", true);
    
    pthread_t tid;
    while(true)
    {
        tcpConnection conn = listenConn.myAccept();
        pthread_create(&tid, NULL, &work, &conn);
    }
}

void *work(void *connfd)
{
    if(pthread_detach(pthread_self()) != 0)
    {
        printf("pthread_detach error\n");
        return NULL;
    }
    myTextChat tc = *(tcpConnection*)connfd;
    tc.work();

    return NULL;
}
