#ifndef _MYLIB_H_
#define _MYLIB_H_

#include <vector>
#include <string>
//class string;

bool isNum(const std::string &str);
std::vector<std::string> split(const std::string &str, const std::string &delimiters = " ");
const std::string strLeft(const std::string &str, int n);

#endif
