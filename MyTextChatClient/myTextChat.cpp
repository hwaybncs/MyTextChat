#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/select.h>
#include "conio.h"
#include "myLib.h"
#include "myTextChat.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
using std::multimap;
using std::stringstream;

myTextChat::myTextChat() {}

void myTextChat::work()
{
    changeUser();
    bool show = true;
    char opt = 0;
    while(true)
    {
        if(show == true)
        {
            cout << endl;
            cout << "Welcome to MyTextChat : " << accountSave << endl;
            cout << endl;
            cout << "R to Read new message" << endl;
            cout << "A to Add friends" << endl;
            cout << "S to Set" << endl;
            cout << "P to Personal data" << endl;
            cout << "F to Friends" << endl;
            cout << "C to Change User" << endl;
            cout << "Q to Quit" << endl;
        }

        opt = getch();

        switch(opt)
        {
            case 'r': case 'R'://Read new message
                readNewMess();
                show = true;
                break;
            case 'a': case 'A'://Add friends
                addFriends();
                show = true;
                break;
            case 's': case 'S'://Set
                set();
                show = true;
                break;
            case 'p': case 'P'://Personal data
                showPersonalData();
                show = true;
                break;
            case 'f': case 'F'://Friends
                showFriends();
                show = true;
                break;
            case 'c': case 'C'://change User
                changeUser();
                show = true;
                break;
            case 'q': case 'Q'://Quit
                quit();
                break;
            default:
                show = false;
                break;
        }
    }
}

void myTextChat::readNewMess()
{
    messCmd = "NEWME ";
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
    conn.myRecv(res, resNum);
    //res[strlen(res)-2] = 0;
    vector<string> strvec(split(res));

    cout << endl;
    int i = 0;
    for(vector<string>::const_iterator it = strvec.begin(); it != strvec.end(); ++it, ++i)
    {
        if(i % 2 == 0)
            cout << i / 2 + 1 << " : ";
        if(*it == "\\r")
            cout << "hopes to add you as a friend";
        else if(*it == "\\r\\nY")
            cout << "has added you as a friend";
        else if(*it == "\\r\\nN")
            cout << "refuses to add you as a friend";
        else 
            cout << *it;

        if(i % 2 == 1)
            cout << endl;
        else 
            cout << "   ";
    }

    if(i == 0)
    {
        cout << "No new news" << endl;
        sleep(3);
        return ;
    }

    cout << endl;
    cout << "Please enter index: ";
    char c;
    while(true)
    {
        c = getch();
        if(c <= '0')
            continue;
        else if(c-'0' >= i/2+1)
            continue;
        else 
            break;
    }

    int index = (c - '0' - 1) * 2;
    if(strvec[index+1] == "\\r")//add friend and response
    {
        cout << endl;
        cout << "Y/N ?";
        string cc;
        cin >> cc;
        cin.clear();
        if(cc == "Y" || cc == "y")
        {
            cout << "Please select the group" << endl;
            int i = 1;
            for(multimap<string, string>::const_iterator it = friGrp.begin(); it != friGrp.end(); )//show groups
            {
                cout << i++ << ": " << it->first << "  ";
                it = friGrp.upper_bound(it->first);
            }
            char gindex;
            while(true)
            {
                gindex = getch();
                if(gindex <= '0')
                    continue;
                if(gindex-'0' >= i)
                    continue;
                else 
                    break;
            }

            string gname;
            int in = 1;
            for(multimap<string, string>::const_iterator it = friGrp.begin();
                    it != friGrp.end(); 
                    ++in)//look for gindex -> gname
            {
                if(in == gindex-'0')
                {
                    gname = it->first;
                    break;
                }
                it = friGrp.upper_bound(it->first);
            }

            messCmd = "AGRFR ";
            messCmd += strvec[index];
            messCmd += " ";
            messCmd += gname;
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
            conn.myRecvCmd(resCmd, resCmdNum);

            if(strcmp(resCmd, "001") == 0)
            {
                //add friend to friGrp
                friGrp.insert(std::make_pair(gname, strvec[index]));
                cout << endl;
                cout << "add success" << endl;
                sleep(3);
                //return ;
            }
        }
        else //N
        {
            messCmd = "NAGRF ";
            messCmd += strvec[index];
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
            conn.myRecvCmd(resCmd, resCmdNum);

            if(strcmp(resCmd, "001") == 0)
            {
                cout << "You have refused the friend request" << endl;
                sleep(3);
                //return ;
            }
        }
    }
    else if(strvec[index+1] == "\\r\\nY")
    {
        cout << "Please select the group" << endl;
        int i = 1;
        for(multimap<string, string>::const_iterator it = friGrp.begin(); it != friGrp.end(); )//show groups
        {
            cout << i++ << ": " << it->first << "  ";
            it = friGrp.upper_bound(it->first);
        }
        char gindex;
        while(true)
        {
            gindex = getch();
            if(gindex <= '0')
                continue;
            if(gindex-'0' >= i)
                continue;
            else 
                break;
        }

        string gname;
        int in = 1;
        for(multimap<string, string>::const_iterator it = friGrp.begin();
                it != friGrp.end(); 
                ++in)//look for gindex -> gname
        {
            if(in == gindex-'0')
            {
                gname = it->first;
                break;
            }
            it = friGrp.upper_bound(it->first);
        }

        messCmd = "RESFR ";
        messCmd += strvec[index];
        messCmd += " ";
        messCmd += gname;
        conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
        conn.myRecvCmd(resCmd, resCmdNum);

        if(strcmp(resCmd, "001") == 0)
        {
            //add friend to friGrp
            friGrp.insert(std::make_pair(gname, strvec[index]));
            cout << "Add friend success" << endl;
            sleep(3);
            //return ;
        }
    }
    else if(strvec[index+1] == "\\r\\nN")
    {
        messCmd = "NRESF ";
        messCmd += strvec[index];
        conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

        cout << endl;
        cout << "The other side has denied your request" << endl;
        sleep(3);
        //return ;
    }
    else //one to one mode
    {
        oneToOne(strvec[index]);
    }
}

void myTextChat::addFriends()
{
    string str;
    while(true)
    {
        cout << endl;
        cout << "Please enter the account" << endl;
        cin >> str;
        cin.clear();
        if(!isNum(str) || str.length() > accountNum)
        {
            cout << "Please enter the correct number" << endl;
            continue;
        }
        if(str == accountSave)
        {
            cout << "This is you" << endl;
            continue;
        }

        messCmd = "EXIST ";
        messCmd += str;
        conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
        conn.myRecvCmd(resCmd, resCmdNum);

        if(strcmp(resCmd, "001") == 0)
            break;
        else 
            cout << "The account does not exist" << endl;
    }

    messCmd = "DATA  ";
    messCmd += str;
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
    conn.myRecv(res, resNum);

    cout << endl;
    cout << str << ": " << endl;
    cout << res << endl;

    /*cout << endl;
    cout << "Please select the group" << endl;
    int i = 1;
    for(multimap<string, string>::const_iterator it = friGrp.begin(); it != friGrp.end(); )
    {
        cout << i++ << ": " << it->first << "  ";
        it = friGrp.upper_bound(it->first);
    }
    */
    cout << endl;
    cout << "Y to add   N to cancel:    " << endl;
    string c;
    cin >> c;
    cin.clear();
    if(c == "Y" || c == "y")
    {
        messCmd = "ADDFR ";
        messCmd += str;
        conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
        conn.myRecvCmd(resCmd, resCmdNum);
        if(strcmp(resCmd, "001") == 0)
        {
            cout << "Please wait for the response" << endl;
            sleep(3);
        }
    }
}

void myTextChat::set()
{
    cout << "= =" << endl;
}

void myTextChat::showPersonalData()
{
    messCmd = "DATA  ";
    messCmd += accountSave;
    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
    conn.myRecv(res, resNum);

    cout << endl;
    cout << accountSave << ": " << endl;
    cout << res << endl;
}

void myTextChat::showFriends()
{
    while(true)
    {
        cout << endl;
        cout << "Please select the group(P to return)" << endl;
        int i = 1;
        for(multimap<string, string>::const_iterator it = friGrp.begin(); it != friGrp.end(); )//show groups
        {
            cout << i++ << ": " << it->first << "  ";
            it = friGrp.upper_bound(it->first);
        }
        char gindex;
        while(true)
        {
            gindex = getch();
            if(gindex == 'P' || gindex == 'p')
                return ;
            if(gindex <= '0')
                continue;
            if(gindex-'0' >= i)
                continue;
            else 
                break;
        }

        cout << endl;
        int in = 1;
        multimap<string, string>::const_iterator itsave;
        int a;
        for(multimap<string, string>::const_iterator it = friGrp.begin();//show friends
                it != friGrp.end(); 
                ++in)
        {
            if(in == gindex-'0')
            {
                itsave = it;
                for(a = 1; it != friGrp.upper_bound(itsave->first); ++it, ++a)
                    cout << a << " : " << it->second << endl;
                break;
            }
            it = friGrp.upper_bound(it->first);
        }

        cout << "Please select the friend(P to return)" << endl;
        char findex;
        while(true)
        {
            findex = getch();
            if(findex == 'P' || findex == 'p')
                break;
            if(findex <= '0')
                continue;
            if(findex-'0' >= a)
                continue;
            else 
                break;
        }
        if(findex == 'P' || findex == 'p')
            continue;

        string fname;
        int fn = 1;
        for(multimap<string, string>::const_iterator it = itsave; 
                it != friGrp.upper_bound(it->first); 
                ++it, ++fn)
        {
            if(fn == findex-'0')
            {
                fname = it->second;
                break;
            }
        }
        oneToOne(fname);
        break;
    }
}

void myTextChat::changeUser()
{
    conn.close();
    tcpConnect c("192.168.159.130", "6666", false);
    conn = c;
    //login mode use UDP?
    cout << endl;
    cout << "******                                                             ******" << endl;
    cout << "******                  Welcome to MyTextChat!                     ******" << endl;
    cout << "******                                                             ******" << endl;
    cout << endl;
    cout << "R to Register      L to Login      Q to Quit" << endl;
    char opt = 0;
    while(true)
    {
        opt = getch();
        if(opt == 'r' || opt == 'R')
        {
            string password;

            messCmd = "USER  ";
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
            conn.myRecvCmd(resCmd, resCmdNum);

            cout << endl;
            cout << "Your account is " << resCmd << endl;
            accountSave = resCmd;

            while(true)
            {
                cout << endl;
                cout << "Please enter your password(less than " << accountNum << " characters)" << endl;
                cin >> password;
                cin.clear();
                if(password.length() > accountNum)
                {
                    cout << "The password is too long" << endl;
                    continue;
                }
                messCmd = "PASSW ";
                messCmd += password;
                conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
                conn.myRecvCmd(resCmd, resCmdNum);
                if(strcmp(resCmd, "001") == 0)
                {
                    cout << "Register success" << endl;
                    sleep(3);
                    break;
                }
            }

            //init friGrp
            friGrp.clear();
            conn.myRecv(res, resNum);
            vector<string> strvec(split(res));
            for(vector<string>::const_iterator it = strvec.begin(); it != strvec.end(); it += 2)
                friGrp.insert(std::make_pair(*it, *(it+1)));
            break;
        }
        else if(opt == 'l' || opt == 'L')
        {
            string account, password;
            //char password[accountNum + 1];
            while(true)
            {
                cout << endl;
                cout << "Please enter your account" << endl;
                cin >> account;
                cin.clear();
                if( !isNum(account) )
                {
                    cout << "Please enter account correctly" << endl;
                    continue;
                }
                if(account.length() > accountNum)
                {
                    cout << "The account does not exist" << endl;
                    continue;
                }

                cout << endl;
                cout << "Please enter your password" << endl;
                cin >> password;
                cin.clear();
                if(password.length() > accountNum)
                {
                    cout << "Password error" << endl;
                    continue;
                }

                messCmd = "LOGIN ";
                messCmd += account;
                messCmd += " ";
                messCmd += password;

                conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
                conn.myRecvCmd(resCmd, resCmdNum);

                if(strcmp(resCmd, "001") == 0)
                {
                    accountSave = account;
                    cout << "login success" << endl;
                    sleep(3);
                    break;
                }
                else 
                    cout << "account or password error" << endl;
                /*
                getch();//delete \n
                cout << endl;
                cout << "Please enter your password" << endl;
                char c;
                int i = 0;
                bool pass = true;
                while((c = getch()) != 13)
                {
                    if(i >= accountNum)//too long
                    {
                        pass = false;
                        continue;
                    }
                    password[i++] = c;
                    cout << c << endl;
                }

                if(pass == false)
                {
                    cout << "Password error" << endl;
                    continue;
                }
                else 
                {
                    password[i] = '\0';
                    break;
                }*/
                
            }
            //init friGrp
            friGrp.clear();
            conn.myRecv(res, resNum);
            vector<string> strvec(split(res));
            for(vector<string>::const_iterator it = strvec.begin(); it != strvec.end(); it += 2)
                friGrp.insert(std::make_pair(*it, *(it+1)));
            break;
        }
        else if(opt == 'q' || opt == 'Q')
        {
            cout << "Goodbye!" << endl;
            exit(0);
        }
    }
}

void myTextChat::quit()
{
    cout << "Goodbye!" << endl;
    exit(0);
}

void myTextChat::oneToOne(const string &otherAccount)
{
    messCmd = "O2OM  ";
    messCmd += otherAccount;

    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
    conn.myRecvCmd(resCmd, resCmdNum);

    /*
    if(strcmp(resCmd, "001") == 0)
    {

    }
    */
    int maxfd;
    fd_set rset;
    string allContent;
    string str;

    /*while(true)
    {
        conn.myRecv(res, resNum);
        if(strcmp(res, "006 \r\n") == 0)
        {
            messCmd = "001";
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
            break;
        }
        if(strLeft(res, 3) == "006")
        {
            messCmd = "001";
            conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);

            res[strlen(res)-2] = 0;
            str = res[4];
            allContent += strFormat(str);
        }
    }*/
    FD_ZERO(&rset);
    while(true)
    {
        cout << endl;
        cout << "********************************************************************************************************" << endl;
        cout << "I to Insert      H to History record       R to return" << endl;
        cout << "Other : " << otherAccount << endl;
        cout << endl;
        cout << allContent << endl;

        FD_SET(fileno(stdin), &rset);
        FD_SET(conn.getFd(), &rset);
        maxfd = std::max(fileno(stdin), conn.getFd()) + 1;
        select(maxfd, &rset, NULL, NULL, NULL);
        
        if(FD_ISSET(conn.getFd(), &rset))//socket is readable
        {
            //gen shang mian yi yang
            conn.myRecv(res, resNum);
            str = &res[4];
            allContent += strFormat(str);
        }

        if(FD_ISSET(fileno(stdin), &rset))//input is readable(bug: enter('\n') will come into this block)
        {
            string s;
            cin >> s;
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if(s == "R" || s == "r")
                break;
            else if(s == "I" || s == "i")
            {
                string istr, s;
                while(std::getline(cin, s))
                    istr += s + "\n";
                cin.clear();

                if(istr.length() >= 7000)
                    istr = strLeft(istr, 6999);
                mess = "SENDM ";
                mess += istr;
                conn.mySend(mess.c_str(), mess.length() + 1);

                while(true)
                {
                    conn.myRecv(res, resNum);
                    if(strcmp(res, "001") == 0)
                        break;
                    str = &res[4];
                    allContent += strFormat(str);
                }
                allContent += "           ";
                allContent += getLocalTime();
                allContent += istr;
            }
            else if(s == "H" || s == "h")
            {
                messCmd = "HISTY ";
                conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
                conn.myRecvCmd(resCmd, resCmdNum);

                int index = 0;
                stringstream sin;
                string s;
                char c = 'P';
                int i;

                while(true)
                {
                    if(c == 'P' || c == 'p')//Previous page
                    {
                        messCmd = "REOLD ";
                        sin.clear();
                        sin << index;
                        sin >> s;
                        messCmd += s;
                        conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
                        for(i = 0; i < 5; )
                        {
                            conn.myRecv(res, resNum);
                            if(strcmp(res, "007") == 0)
                                break;
                            if(strLeft(res, 3) == "006")
                            {
                                str = &res[4];
                                allContent += strFormat(str);
                            }
                            else if(res[strlen(res)-1] == 'M')
                            {
                                cout << localStrFormat(res) << endl;
                                ++index;
                                ++i;
                            }
                            else if(res[strlen(res)-1] == 'Y')
                            {
                                cout << strFormat(res) << endl;
                                ++index;
                                ++i;
                            }
                        }
                        if(i == 0)
                            cout << "No more message" << endl;
                    }
                    else if(c == 'N' || c == 'n')//Next page
                    {
                        messCmd = "NEOLD ";
                        sin.clear();
                        sin << index;
                        sin >> s;
                        messCmd += s;
                        conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
                        for(i = 0; i < 5; )
                        {
                            conn.myRecv(res, resNum);
                            if(strcmp(res, "007") == 0)
                                break;
                            if(strLeft(res, 3) == "006")
                            {
                                str = &res[4];
                                allContent += strFormat(str);
                            }
                            else if(res[strlen(res)-1] == 'M')
                            {
                                cout << localStrFormat(res) << endl;
                                --index;
                                ++i;
                            }
                            else if(res[strlen(res)-1] == 'Y')
                            {
                                cout << strFormat(res) << endl;
                                --index;
                                ++i;
                            }
                        }
                        if(i == 0)
                            cout << "No more message" << endl;
                    }
                    else if(c == 'R' || c == 'r')//return
                    {
                        break;
                    }

                    cout << "N to Next page and P to Previous page(R to return)" << endl;
                    c = getch();
                }
                messCmd = "HISYQ ";
                conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
                conn.myRecvCmd(resCmd, resCmdNum);
            }
            else 
            {
                cout << "Please enter correctly" << endl;
            }
        }
    }

    messCmd = "O2OMQ ";

    conn.mySendCmd(messCmd.c_str(), messCmd.length() + 1);
    conn.myRecvCmd(resCmd, resCmdNum);
}

const std::string myTextChat::strFormat(const std::string &str)
{
    string::size_type n = str.length();
    string s = str.substr(0, n-5);

    string::size_type pos = s.find_first_of(" ");
    pos = s.find_first_of(" ", pos+1);
    s[pos] = '\n';
    return s;
}

const std::string myTextChat::localStrFormat(const std::string &str)
{
    string::size_type n = str.length();
    string s = str.substr(0, n-5);
    
    string::size_type pos = s.find_first_of(" ");
    pos = s.find_first_of(" ", pos+1);
    s[pos] = '\n';
    string fins("           ");
    fins += s;
    return fins;
}

const std::string myTextChat::getLocalTime()
{
    time_t t;
    time(&t);
    localtime_r(&t, &a);

    char da[25];
    snprintf(da, 25, "%d-%d-%d %d:%d:%d\n", 1900+a.tm_year, 1+a.tm_mon, a.tm_mday, a.tm_hour, a.tm_min, a.tm_sec);
    return da;
}
