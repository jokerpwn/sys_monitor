#include "sys.h"

sys::sys():host_name(""),run_time(0),version("")
{
    update_sys_info();
}
void sys::update_sys_info(){
    ifstream version_file("/proc/version");
    if(!version_file.is_open()){
        cout<<"error open version file!"<<endl;
        return;
    }
    getline(version_file,version);
    version_file.close();

    ifstream host_info("/proc/sys/kernel/hostname");
    if(!host_info.is_open()){
        cout<<"error open cpu file!"<<endl;
        return;
    }

    getline(host_info,host_name);
    host_info.close();
}

double sys::get_time_info(){
    ifstream time_info("/proc/uptime");
    if(!time_info.is_open()){
        cout<<"error open time info"<<endl;
        return run_time;
    }
    string str;
    time_info>>str;
    time_info.close();

    run_time=atof(str.c_str());

    time_t tt;
    //cur_time
    time(&tt);
    tm* t= localtime(&tt);
    sprintf(cur_time,"%d-%02d-%02d %02d:%02d:%02d\n",
     t->tm_year + 1900,
     t->tm_mon + 1,
     t->tm_mday,
     t->tm_hour,
     t->tm_min,
     t->tm_sec);

     tt-=run_time;
     t=localtime(&tt);
     sprintf(start_time,"%d-%02d-%02d %02d:%02d:%02d\n",
      t->tm_year + 1900,
      t->tm_mon + 1,
      t->tm_mday,
      t->tm_hour,
      t->tm_min,
      t->tm_sec);

    return run_time;
}

