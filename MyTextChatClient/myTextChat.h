#ifndef _MYTEXTCHAT_H_
#define _MYTEXTCHAT_H_

#include <string>
#include <map>
#include "myConnection.h"

class myTextChat
{
public:
    myTextChat();
    void work();

    void readNewMess();
    void addFriends();
    void set();
    void showPersonalData();
    void showFriends();
    void changeUser();
    void quit();

private:
    void oneToOne(const std::string &otherAccount);
    const std::string strFormat(const std::string &str);
    const std::string localStrFormat(const std::string &str);
    const std::string getLocalTime();

    enum {
        accountNum = 20,//account or password size
        resCmdNum = 100,//cmd size: 99
        resNum = 7000//long respond size: 6999
    };

    tcpConnection conn;
    std::string accountSave;
    std::multimap<std::string, std::string> friGrp;
    struct tm a;

    std::string messCmd;
    std::string mess;
    char resCmd[resCmdNum];
    char res[resNum];
};

#endif
