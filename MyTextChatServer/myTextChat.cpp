#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>
#include "../myFuntion/myLib.h"
#include "myPair.h"
#include "myTextChat.h"

using namespace std;

myTextChat::myTextChat(const tcpConnection &c) : conn(c) 
{
    pthread_mutex_lock(&accountMutex);

    if(accountIncre == 0)
    {
        ifstream fin("data/accountNum", ios::binary);
        if(!fin)//file not exist
            accountBase = base;
        else 
        {
            fin.read((char*)&accountBase, sizeof(accountBase));
            fin.close();
            if(accountBase == 0)//file empty
                accountBase = base;
        }
    }

    pthread_mutex_unlock(&accountMutex);

    //init database
    connection = mysql_init(NULL);
    if(connection == NULL)
    {
        cout << "Error: " << mysql_error(connection);
        exit(1);
    }
    connection = mysql_real_connect(connection, "localhost", "root", "root", "myDB", 0, NULL, 0);
    if(connection == NULL)
    {
        cout << "Error: " << mysql_error(connection);
        exit(1);
    }
}

myTextChat::~myTextChat()
{
    if(connection != NULL)
        mysql_close(connection);
    //~...global variable store
}

void myTextChat::work()
{
    string str;
    while(true)
    {
        if(conn.myRecvCmd(resCmd, resCmdNum) == 0)//eof
        {
            pthread_exit(0);
        }
        str = strLeft(resCmd, 5);

        if(str == "USER ")
            user();
        else if(str == "PASSW")
            passw();
        else if(str == "LOGIN")
            login();
        else if(str == "EXIST")
            exist();
        else if(str == "DATA ")
            data();
        else if(str == "ADDFR")
            addfr();
        else if(str == "NEWME")
            newme();
        else if(str == "AGRFR")
            agrfr();
        else if(str == "NAGRF")
            nagrf();
        else if(str == "RESFR")
            resfr();
        else if(str == "NRESF")
            nresf();
        else if(str == "O2OM ")
            o2om();
        else 
            continue;
    }
}

void myTextChat::luckyNum()
{
    while(true)
    {
        int count = 0;
        ++accountIncre;
        
        long sum = accountBase + accountIncre;//count the number of bits
        while((sum /= 10) > 0)
            ++count;
        ++count;

        sum = accountBase + accountIncre;
        while(sum % 10 == 0)
        {
            sum /= 10;
            --count;
        }
        if(count == 1)//lucky num
            accountList.push_back(pair<char*, bool>("asd"));
        else 
            break;
    }
}

const string myTextChat::getLocalTime()
{
    time_t t;
    time(&t);
    localtime_r(&t, &a);

    char da[25];
    snprintf(da, 25, "%d-%d-%d %d:%d:%d\n", 1900+a.tm_year, 1+a.tm_mon, a.tm_mday, a.tm_hour, a.tm_min, a.tm_sec);
    return da;
}

void myTextChat::user()
{
    pthread_mutex_lock(&accountMutex);

    if(accountIncre == 0)
    {
        ifstream fin("data/accountNum", ios::binary);
        if(!fin)//file not exist
            accountBase = base;
        else 
        {
            fin.read((char*)&accountBase, sizeof(accountBase));
            fin.close();
            if(accountBase == 0)//file empty
                accountBase = base;
        }
    }

    if(accountList2.empty() == false)
    {
        ostringstream ss;
        ss << accountList2.front();
        accountList2.pop_front();
        messCmd = ss.str();
        conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

        pthread_mutex_unlock(&accountMutex);
        return ;
    }

    luckyNum();

    accountList.push_back(pair<char*, bool>());
    ostringstream ss;
    ss << accountBase + accountIncre;//account is accountBase+accountIncre
    messCmd = ss.str();
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

    accountSave = messCmd;
    accountSaveNum = accountBase + accountIncre;

    pthread_mutex_unlock(&accountMutex);
}

void myTextChat::passw()
{
    pthread_mutex_lock(&accountMutex);

    int i = accountSaveNum - accountBase;
    list<pair<char*, bool> >::iterator it;
    for(it = accountList.begin(); i > 1; --i, ++it);

    it->setFirst(&resCmd[6]);
    it->setSecond(true);

    if(it == accountList.begin())//write to file "accountNum" and "account"
    {
        long ac;
        char ps[passwordNum+1];

        ofstream out("data/account", ios::binary|ios::app);
        for(; it != accountList.end(); )
        {
            if(it->second == false)
                break;
            //~...
            ac = accountBase + 1;
            snprintf(ps, passwordNum+1, "%s", it->first);
            out.write((char*)&ac, sizeof(ac));//account
            out.write(ps, sizeof(ps));//password

            it = accountList.erase(it);
            ++accountBase;
            --accountIncre;
        }

        if(it == accountList.end() && accountIncre == 0)
        {
            ofstream out2("data/accountNum", ios::binary);
            out2.write((char*)&accountBase, sizeof(accountBase));
        }
    }

    messCmd = "001";
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

    pthread_mutex_unlock(&accountMutex);

    //create accountSave table
    string sqlText("create table record");
    sqlText += accountSave;
    sqlText += "(account varchar(15), time datetime, new bit, text varchar(8000));";

    mysql_query(connection, sqlText.c_str());

    //create friendaccountSave table
    sqlText = "create table friend";
    sqlText += accountSave;
    sqlText += "(grpName varchar(20), name varchar(15) primary key)";

    mysql_query(connection, sqlText.c_str());

    //insert default group
    sqlText = "insert into friend";
    sqlText += accountSave;
    sqlText += " values('default', 'asd')";

    mysql_query(connection, sqlText.c_str());

    mess = "default asd";
    conn.mySend(mess.c_str(), mess.length() + 1);
}

void myTextChat::login()
{
    pthread_mutex_lock(&accountMutex);

    string pa;
    stringstream ss;
    ss << &resCmd[6] << " " << &resCmd[6];
    ss >> accountSave >> pa >> accountSaveNum;

    long ac2;
    char pa2[passwordNum+1];
    if(accountSaveNum <= accountBase)//find in the file "account"
    {
        ifstream in("data/account", ios::binary);
        if(!in)
        {
            messCmd = "003";
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

            pthread_mutex_unlock(&accountMutex);
            return ;
        }

        long n = accountSaveNum - base - 1;
        in.seekg(n*(sizeof(long)+passwordNum+1), ios::beg);
        if( !(in.read((char*)&ac2, sizeof(ac2))) )//eof / n<0
        {
            messCmd = "003";
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

            pthread_mutex_unlock(&accountMutex);
            return ;
        }
        if(accountSaveNum != ac2)
            exit(-1);

        in.read(pa2, sizeof(pa2));
        
        if(pa != pa2) //password error
        {
            messCmd = "004";
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

            pthread_mutex_unlock(&accountMutex);
            return ;
        }
    }
    else //find in the accountList
    {
        long n = accountSaveNum - accountBase;
        list<pair<char*, bool> >::const_iterator it;
        for(it = accountList.begin(); it != accountList.end(); ++it)
            if(--n == 0)
                break;

        if(it == accountList.end())//not exist
        {
            messCmd = "003";
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

            pthread_mutex_unlock(&accountMutex);
            return ;
        }
        if(pa != it->first) //password error
        {
            messCmd = "004";
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

            pthread_mutex_unlock(&accountMutex);
            return ;
        }
    }

    pthread_mutex_unlock(&accountMutex);

    //password correct
    messCmd = "001";
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

    mess = "";
    //string sqlText("select * from friendaccountSave)
    string sqlText("select * from friend");
    sqlText += accountSave;

    mysql_query(connection, sqlText.c_str());
    result = mysql_use_result(connection);
    while(true)
    {
        row = mysql_fetch_row(result);
        if(row == 0)
            break;
        for(int j = 0; j < mysql_num_fields(result); ++j)
        {
            mess += row[j];
            mess += " ";
        }
    }
    mysql_free_result(result);

    conn.mySend(mess.c_str(), mess.length() + 1);
}

void myTextChat::exist()
{
    pthread_mutex_lock(&accountMutex);

    long account;
    stringstream ss;
    ss << &resCmd[6];
    ss >> account;

    long ac2;
    if(account <= accountBase)//find in the file "account"
    {
        ifstream in("data/account", ios::binary);
        if(!in)
        {
            messCmd = "003";
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

            pthread_mutex_unlock(&accountMutex);
            return ;
        }

        long n = account - base - 1;
        in.seekg(n*(sizeof(long)+passwordNum+1), ios::beg);
        if( !(in.read((char*)&ac2, sizeof(ac2))) )//eof / n<0
        {
            messCmd = "003";
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

            pthread_mutex_unlock(&accountMutex);
            return ;
        }
        if(account != ac2)
            exit(-1);

        messCmd = "001";
        conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

        pthread_mutex_unlock(&accountMutex);
        return ;
    }
    else //find in the accountList
    {
        long n = account - accountBase;
        list<pair<char*, bool> >::const_iterator it;
        for(it = accountList.begin(); it != accountList.end(); ++it)
            if(--n == 0)
                break;

        if(it == accountList.end())//not exist
        {
            messCmd = "003";
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

            pthread_mutex_unlock(&accountMutex);
            return ;
        }

        messCmd = "001";
        conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

        pthread_mutex_unlock(&accountMutex);
        return ;
    }

    pthread_mutex_unlock(&accountMutex);
}

void myTextChat::data()
{
    string fileName("data/");
    fileName += &resCmd[6];
    fileName += "_personalData";

    pthread_mutex_lock(&dataMutex);
    ifstream in(fileName.c_str());
    if(!in)
    {
        ofstream out(fileName.c_str());
        string s("Name: Unknown        Sex: Unknown        Age: Unknown        ");
        out << s;

        in.open(fileName.c_str());
    }
    string s;
    mess = "";
    while(std::getline(in, s))
       mess += s;
    pthread_mutex_unlock(&dataMutex);

    conn.mySend(mess.c_str(), mess.length() + 1);
}

void myTextChat::addfr()
{
    string sqlText("insert into record");
    sqlText += &resCmd[6];
    sqlText += " values('";
    sqlText += accountSave;
    sqlText += "','";
    sqlText += getLocalTime();
    sqlText += "',1,'\\\\r')";

    mysql_query(connection, sqlText.c_str());

    messCmd = "001";
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
}

void myTextChat::newme()
{
    mess = "";
    //string sqlText("select account, COUNT(account) from (select account, text from record(accountSave) where new = 1) 
    //                   as re0 where text NOT IN ('\\\\r', '\\\\r\\\\nY', '\\\\r\\\\nN') group by account")
    string sqlText("select account, COUNT(account) from (select account, text from record");
    sqlText += accountSave;
    sqlText += " where new = 1)as re0 where text NOT IN ('\\\\r', '\\\\r\\\\nY', '\\\\r\\\\nN') group by account";

    mysql_query(connection, sqlText.c_str());
    result = mysql_use_result(connection);
    while(true)
    {
        row = mysql_fetch_row(result);
        if(row == 0)
            break;
        for(int j = 0; j < mysql_num_fields(result); ++j)
        {
            mess += row[j];
            mess += " ";
        }
    }
    mysql_free_result(result);

    //sqlText("select DISTINCT account, text from record100000002 where new = 1 and text in ('\\\\r', '\\\\r\\\\nY', '\\\\r\\\\nN');")
    sqlText = "select DISTINCT account, text from record";
    sqlText += accountSave;
    sqlText += " where new = 1 and text in ('\\\\r', '\\\\r\\\\nY', '\\\\r\\\\nN')";

    mysql_query(connection, sqlText.c_str());
    result = mysql_use_result(connection);
    while(true)
    {
        row = mysql_fetch_row(result);
        if(row == 0)
            break;
        for(int j = 0; j < mysql_num_fields(result); ++j)
        {
            mess += row[j];
            mess += " ";
        }
    }
    mysql_free_result(result);

    conn.mySend(mess.c_str(), mess.length() + 1);
}

void myTextChat::agrfr()
{
    int i;
    for(i = 6; i < strlen(resCmd); ++i)
        if(resCmd[i] == ' ')
            break;
    ++i;//i -> gname
    char otherAccount[15];
    strncpy(otherAccount, &resCmd[6], i-6-1);
    otherAccount[i-6-1] = 0;

    //sqlText("delete from recordaccountSave where account = 'otherAccount' and text = '\\\\r'")
    string sqlText("delete from record");
    sqlText += accountSave;
    sqlText += " where account = '";
    sqlText += otherAccount;
    sqlText += "' and text = '\\\\r'";

    mysql_query(connection, sqlText.c_str());

    //sqlText("insert into recordotherAccount values('accountSave', 'getLocalTime()', 1, '\\\\r\\\\nY')")
    sqlText = "insert into record";
    sqlText += otherAccount;
    sqlText += " values('";
    sqlText += accountSave;
    sqlText += "','";
    sqlText += getLocalTime();
    sqlText += "',1,'\\\\r\\\\nY')";

    mysql_query(connection, sqlText.c_str());

    //sqlText("insert into friendaccountSave values('&resCmd[i]', 'otherAccount')")
    sqlText = "insert into friend";
    sqlText += accountSave;
    sqlText += " values('";
    sqlText += &resCmd[i];
    sqlText += "', '";
    sqlText += otherAccount;
    sqlText += "')";

    mysql_query(connection, sqlText.c_str());

    messCmd = "001";
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
}

void myTextChat::nagrf()
{
    //sqlText("delete from recordaccountSave where account = '&resCmd[6]' and text = '\\\\r'")
    string sqlText("delete from record");
    sqlText += accountSave;
    sqlText += " where account = '";
    sqlText += &resCmd[6];
    sqlText += "' and text = '\\\\r'";

    mysql_query(connection, sqlText.c_str());

    //sqlText("insert into record&resCmd[6] values('accountSave', 'getLocalTime()', 1, '\\\\r\\\\nN')")
    sqlText = "insert into record";
    sqlText += &resCmd[6];
    sqlText += " values('";
    sqlText += accountSave;
    sqlText += "','";
    sqlText += getLocalTime();
    sqlText += "',1,'\\\\r\\\\nN')";

    mysql_query(connection, sqlText.c_str());

    messCmd = "001";
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
}

void myTextChat::resfr()
{
    int i;
    for(i = 6; i < strlen(resCmd); ++i)
        if(resCmd[i] == ' ')
            break;
    ++i;//i -> gname
    char otherAccount[15];
    strncpy(otherAccount, &resCmd[6], i-6-1);
    otherAccount[i-6-1] = 0;

    //sqlText("delete from recordaccountSave where account = 'otherAccount' and text = '\\\\r\\\\nY'")
    string sqlText("delete from record");
    sqlText += accountSave;
    sqlText += " where account = '";
    sqlText += otherAccount;
    sqlText += "' and text = '\\\\r\\\\nY'";

    mysql_query(connection, sqlText.c_str());

    //sqlText("insert into friendaccountSave values('&resCmd[i]', 'otherAccount')")
    sqlText = "insert into friend";
    sqlText += accountSave;
    sqlText += " values('";
    sqlText += &resCmd[i];
    sqlText += "', '";
    sqlText += otherAccount;
    sqlText += "')";

    mysql_query(connection, sqlText.c_str());

    messCmd = "001";
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
}

void myTextChat::nresf()
{
    //sqlText("delete from recordaccountSave where account = '&resCmd[6]' and text = '\\\\r\\\\nN'")
    string sqlText("delete from record");
    sqlText += accountSave;
    sqlText += " where account = '";
    sqlText += &resCmd[6];
    sqlText += "' and text = '\\\\r\\\\nN'";

    mysql_query(connection, sqlText.c_str());
}

void myTextChat::o2om()
{
    otherAccount = &resCmd[6];
    string str;

    messCmd = "001";
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);


    fd_set rset;
    struct timeval tv;
    int reva;

    while(true)
    {
        FD_ZERO(&rset);
        FD_SET(conn.getFd(), &rset);

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        reva = select(conn.getFd()+1, &rset, NULL, NULL, &tv);
        if(reva < 0)
        {
            if(errno == EINTR)
                continue;
            else 
            {
                cout << "select error" << endl;
                exit(-1);
            }
        }
        else if(reva == 0)//timeout
        {
            //string sqlText("select time, text from recordaccountSave where account = 'otherAccount' and new = 1 and 
            //text NOT IN ('\\\\r', '\\\\r\\\\nY', '\\\\r\\\\nN')")
            string sqlText("select time, text from record");
            sqlText += accountSave;
            sqlText += " where account = '";
            sqlText += otherAccount;
            sqlText += "' and new = 1 and text NOT IN ('\\\\r', '\\\\r\\\\nY', '\\\\r\\\\nN')";

            mysql_query(connection, sqlText.c_str());
            result = mysql_store_result(connection);
            while(true)
            {
                row = mysql_fetch_row(result);
                if(row == 0)
                    break;
                mess = "006";
                for(int j = 0; j < mysql_num_fields(result); ++j)
                {
                    mess += " ";
                    mess += row[j];
                }
                conn.mySend(mess.c_str(), mess.length() + 1);

                //~...bug and inefficiency
                //sqlText("update recordaccountSave set new = 0 where account = 'otherAccount' and time = 'row[0]' and
                //new = 1")
                sqlText = "update record";
                sqlText += accountSave;
                sqlText += " set new = 0 where account = '";
                sqlText += otherAccount;
                sqlText += "' and time = '";
                sqlText += row[0];
                sqlText += "' and new = 1";

                mysql_query(connection, sqlText.c_str());
            }
            mysql_free_result(result);
        }
        else 
        {
            if(conn.myRecv(res, resNum) == 0)//eof
            {
                pthread_exit(0);
            }
            str = strLeft(res, 5);

            if(str == "O2OMQ")
                break;
            else if(str == "SENDM")
                sendm();
            else if(str == "HISTY")
                histy();
            else 
                continue;
        }
    }
    o2omq();
}

void myTextChat::o2omq()
{
    messCmd = "001";
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
}

void myTextChat::sendm()
{
    //sqlText("insert into recordaccountSave values('otherAccount', 'getLocalTime()', 0, '&res[6]')")
    string sqlText = "insert into record";
    sqlText += accountSave;
    sqlText += " values('";
    sqlText += otherAccount;
    sqlText += "', '";
    sqlText += getLocalTime();
    sqlText += "', 0, '";
    sqlText += &res[6];
    sqlText += "\\\\r\\\\nM')";

    mysql_query(connection, sqlText.c_str());

    //sqlText("insert into recordotherAccount values('accountSave', 'getLocalTime()', 1, '&res[6]')")
    sqlText = "insert into record";
    sqlText += otherAccount;
    sqlText += " values('";
    sqlText += accountSave;
    sqlText += "', '";
    sqlText += getLocalTime();
    sqlText += "', 1, '";
    sqlText += &res[6];
    sqlText += "\\\\r\\\\nY')";

    mysql_query(connection, sqlText.c_str());

    messCmd = "001";
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
}

void myTextChat::histy()
{
    messCmd = "001";
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
    
    //sqlText("select time, text from recordaccountSave where account = 'otherAccount' and new = 0
    // and text NOT IN ('\\\\r', '\\\\r\\\\nY', '\\\\r\\\\nN')")
    string sqlText = "select time, text from record";
    sqlText += accountSave;
    sqlText += " where account = '";
    sqlText += otherAccount;
    sqlText += "' and new = 0 and text NOT IN ('\\\\r', '\\\\r\\\\nY', '\\\\r\\\\nN')";

    mysql_query(connection, sqlText.c_str());
    result = mysql_store_result(connection);

    string str;
    int wchi = 0;
    bool rve = false;
    while(true)
    {
        if(conn.myRecvCmd(resCmd, resCmdNum) == 0)//eof
        {
            pthread_exit(0);
        }
        str = strLeft(resCmd, 5);

        if(str == "REOLD")
        {
            if(wchi != 0)//last is NEOLD
            {
                rve = true;
                wchi = 0;
            }
            reold(rve);
            rve = false;
        }
        else if(str == "NEOLD")//last is REOLD
        {
            if(wchi != 1)
            {
                rve = true;
                wchi = 1;
            }
            neold(rve);
            rve = false;
        }
        else if(str == "HISYQ")
        {
            messCmd = "001";
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
            break;
        }
        else 
            continue;
    }
    mysql_free_result(result);
}

void myTextChat::reold(bool rve)
{
    my_ulonglong rowNum = mysql_num_rows(result);
    stringstream in(&resCmd[6]);
    long ind;
    in >> ind;
    ind = rowNum - ind - 1;//reverse
    if(rve == true)
        --ind;
    if(ind < 0 || ind >= rowNum)
    {
        mess = "007";
        conn.mySend(mess.c_str(), mess.length() + 1);
        return ;
    }
    int ci;
    if(ind - 4 < 0)//get the start
    {
        ci = ind + 1;
        ind = 0;
    }
    else 
    {
        ind -= 4;
        ci = 5;
    }
    my_ulonglong index = ind;
    mysql_data_seek(result, index);
    int i;
    for(i = 0; i < ci; ++i)
    {
        row = mysql_fetch_row(result);
        if(row == 0)
            break;
        mess = "";
        for(int j = 0; j < mysql_num_fields(result); ++j)
        {
            if(j != 0)
                mess += " ";
            mess += row[j];
        }
        conn.mySend(mess.c_str(), mess.length() + 1);
    }
    if(i < 5)
    {
        mess = "007";
        conn.mySend(mess.c_str(), mess.length() + 1);
    }
}

void myTextChat::neold(bool rve)
{
    my_ulonglong rowNum = mysql_num_rows(result);
    stringstream in(&resCmd[6]);
    long ind;
    in >> ind;
    ind = rowNum - ind - 1;//reverse
    if(rve == true)
        ++ind;
    if(ind < 0 || ind >= rowNum)
    {
        mess = "007";
        conn.mySend(mess.c_str(), mess.length() + 1);
        return ;
    }
    my_ulonglong index = ind;
    mysql_data_seek(result, index);
    int i;
    for(i = 0; i < 5; ++i)
    {
        row = mysql_fetch_row(result);
        if(row == 0)
            break;
        mess = "";
        for(int j = 0; j < mysql_num_fields(result); ++j)
        {
            if(j != 0)
                mess += " ";
            mess += row[j];
        }
        conn.mySend(mess.c_str(), mess.length() + 1);
    }
    if(i < 5)
    {
        mess = "007";
        conn.mySend(mess.c_str(), mess.length() + 1);
    }
}
