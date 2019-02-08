#ifndef MEMORY_H
#define MEMORY_H

#include"util.h"
using namespace::std;
struct proc_mem_info{
    string name;
    pid_t pid;
    string user;
    string thread_num;
    //total program size
    unsigned long size;
    //resident set size
    unsigned long rss;
    float mem_rate;
    friend istream& operator >>(istream& in,proc_mem_info& obj);
};
class memory
{
private:
    unsigned long mem_total;
    unsigned long mem_free;
    unsigned long buffers;
    unsigned long swapped;
    float use_rate;
    unsigned long proc_num;
public:
    memory();
    //获取系统内存基本信息
    void get_mem_info();
    //获取进程内存基本信息
    unsigned long get_proc_info(proc_mem_info *);
    float get_use_rate(){return use_rate;}
    unsigned long get_mem_total(){return mem_total;}
    unsigned long get_mem_used(){return mem_total-mem_free;}
    unsigned long get_buffer(){return buffers;}
    unsigned long get_swapped(){return swapped;}
};


#endif // MEMORY_H
