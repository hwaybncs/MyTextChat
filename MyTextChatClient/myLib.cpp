#include <string>
#include <sstream>
#include "myLib.h"

bool isNum(const std::string &str)
{
    if(str[0] == '-' || str[0] == '+')
        return false;
    std::stringstream sin(str);  
    long l;
    char c;  
    if(!(sin >> l))  
        return false;
    if (sin >> c) 
        return false;
    return true;  
}

std::vector<std::string> split(const std::string &str, const std::string &delimiters)
{
    std::vector<std::string> v;

    std::string::size_type pos = str.find_first_not_of(delimiters);
    std::string::size_type nextPos = str.find_first_of(delimiters, pos);

    while(pos != std::string::npos)
    {
        v.push_back(str.substr(pos, nextPos-pos));
        pos = str.find_first_not_of(delimiters, nextPos);
        nextPos = str.find_first_of(delimiters, pos);
    }
    return v;
}

const std::string strLeft(const std::string &str, int n)
{
    return str.substr(0, n);
}
