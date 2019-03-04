#ifndef CPU_H
#define CPU_H

#include"util.h"
#define PROC_MAX_NUM 2048
#define PID_POS 1
#define NAME_POS 2
#define STATE_POS 3
#define PPID_POS 4
#define PRIORITY_POS 18
#define TIME_POS 14
#define THREAD_POS 20
#define INFO_LENGTH 52

using namespace std;
struct cpu_time_info{
    char str[20];
    unsigned long usr;
    unsigned long nice;
    unsigned long sys;
    unsigned long idle;
    unsigned long io_wait;
    unsigned long irq;
    unsigned long soft_irq;
    unsigned long steal;
    double  sum() const;
    double non_idle() const;
    friend istream& operator >>(istream& in,cpu_time_info& obj);
};
struct proc_cpu_info{
    string name;
    pid_t pid;
    string task_state;
    string ppid;//parent pid
    string priority;
    unsigned long thread_num;
    string user;

    unsigned long utime;  //user time
    unsigned long stime;  //kernel time
    unsigned long cutime; //all user time
    unsigned long cstime; //all dead time
    unsigned long  time_sum() const;

    double cpu_rate;
    friend istream& operator >>(istream& in,proc_cpu_info& obj);
};
typedef map<pid_t, proc_cpu_info> storedProcType;

class cpu
{
private:
    double sys_rate;
    double usr_rate;
    double cpu_use_rate;
    unsigned long proc_num;
    unsigned long thread_num;
    string model_name;
    int processor;
    string freq;
    string core_num;
public:


    cpu_time_info cpu_time;
    cpu();
    unsigned long get_total_time(){return cpu_time.sum();}
    void get_snapshot(cpu_time_info&);
    double get_cpu_use(const cpu_time_info &,const cpu_time_info&);
    double get_sys_rate(const cpu_time_info &,const cpu_time_info&);
    double get_usr_rate(const cpu_time_info &,const cpu_time_info&);
    string& get_cpu_name(){ return model_name;}
    string& get_core_num(){ return core_num;}
    string& get_frequency(){ return freq;}
    int get_processor(){ return processor;}
    unsigned long get_proc_info(proc_cpu_info *);
    void get_proc_full(proc_cpu_info *);
    double calc_cpu_rate(const proc_cpu_info* before, const proc_cpu_info* after,const unsigned long long &cpuTime);
    unsigned long get_total_thread(){return thread_num;}
    unsigned long get_total_procs(){return proc_num;}
};


#endif // CPU_H
