#include <utility>
#include <cstring>
#include <algorithm>

namespace std{

template <>
struct pair<char*, bool>
{
    typedef char* first_type;
    typedef bool second_type;

    char* first;
    bool second;

    pair() : first(0), second(0) {}
    pair(const char *c, bool b = false) : second(b)
    {
        if(c == 0)
        {
            first = 0;
            return ;
        }
        first = new char[strlen(c) + 1];
        strncpy(first, c, strlen(c) + 1);
    }

    pair(const pair<char*, bool> &p) : second(p.second)
    {
        if(p.first == 0)
        {
            first = 0;
            return ;
        }
        first = new char[strlen(p.first) + 1];
        strncpy(first, p.first, strlen(p.first) + 1);
    }

    pair<char*, bool>& operator =(const pair<char*, bool> &p)
    {
        pair<char*, bool> temp(p);
        swap(temp);
        return *this;
    }

    ~pair()
    {
        delete []first;
    }

    void swap(pair<char*, bool> &p)
    {
        std::swap(first, p.first);
        std::swap(second, p.second);
    }

    void setFirst(const char *c)
    {
        if(c == 0)
        {
            first = 0;
            return ;
        }
        char *temp = first;
        first = new char[strlen(c) + 1];
        strncpy(first, c, strlen(c) + 1);

        delete []temp;
    }

    void setSecond(bool b)
    {
        second = b;
    }
};

}
