#ifndef UTIL_H
#define UTIL_H
#include <map>
#include <sys/stat.h>
#include <pwd.h>
#include<iostream>
#include<string>
#include<fstream>
#include<regex>
#include<cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include<string>
#include<QString>
#include<QIcon>
#include<unordered_map>
#include"hashqstring.h"

using namespace std;
class util
{
public:
    static map<char,const string> state_map;
    util();
    static string file_user(const struct stat* file_message);
    static QString getProcessCmdline(pid_t pid);
    static QIcon getProcessIconFromName(QString procName, std::unordered_map<QString, QIcon> &processIconMapCache);

};


#endif // UTIL_H
