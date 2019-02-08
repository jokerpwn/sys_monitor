#ifndef SYS_H
#define SYS_H
#include<iostream>
#include<fstream>
#include<string>
#include<ctime>

using namespace std;
class sys
{
private:
    string host_name;
    double run_time;
    char cur_time[30];
    char start_time[30];
    string version;
public:
    sys();
    string& get_host(){return host_name;}
    double get_time_info();
    string& get_version(){return version;}
    void update_sys_info();
    string get_start_time(){return start_time;}
    string get_cur_time(){return cur_time;}
};

#endif // SYS_H
