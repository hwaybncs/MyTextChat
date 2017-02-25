#ifndef _MYTEXTCHAT_H_
#define _MYTEXTCHAT_H_

#include <string>
#include <list>
#include <utility>
#include <time.h>
#include <mysql/mysql.h>
#include "myConnection.h"

extern long accountBase;
extern long accountIncre;
extern std::list<std::pair<char*, bool> > accountList;//password, bool(accountBase+index+1 is account)
extern std::list<long> accountList2;

extern pthread_mutex_t accountMutex;
extern pthread_mutex_t dataMutex;

class myTextChat
{
public:
    myTextChat(const tcpConnection &c);
    ~myTextChat();
    void work();

    void user();
    void passw();
    void login();
    void exist();
    void data();
    void addfr();
    void newme();
    void agrfr();
    void nagrf();
    void resfr();
    void nresf();
    void o2om();
    void o2omq();
    void sendm();
    void reold(bool);
    void neold(bool);
    void histy();

private:
    void luckyNum();
    const std::string getLocalTime();

    enum 
    {
        base = 100000000,
        //accountNum = 13,//account size
        passwordNum = 20,//password size
        resCmdNum = 100,//cmd size is 99
        resNum = 7000//long respond size is 6999
    };

    tcpConnection conn;
    std::string accountSave;
    long accountSaveNum;
    std::string otherAccount;
    struct tm a;

    MYSQL *connection;
    MYSQL_RES *result;
    MYSQL_ROW row;

    std::string messCmd;
    std::string mess;
    char resCmd[resCmdNum];
    char res[resNum];
};

#endif
