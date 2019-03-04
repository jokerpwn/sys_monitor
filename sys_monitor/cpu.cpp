#include "cpu.h"
#include<cmath>
istream& operator>>(istream&in,cpu_time_info &obj){
    //http://unix.stackexchange.com/q/178045/20626
    //io_wait+idle-->real idle
    //guest and guestnice since they are already included usr&nice
    in>>obj.str>>obj.usr>>obj.nice>>obj.sys>>obj.idle>>
            obj.io_wait>>obj.irq>>obj.soft_irq
            >>obj.steal;
    return in;
}
istream& operator>>(istream&in,proc_cpu_info &obj){
    char r;
    string str;
    for(int i=1;i<=INFO_LENGTH;i++)
    {
        switch (i)
        {
        case PID_POS:
            in>>obj.pid;
            break;
        case NAME_POS:
            in>>obj.name;
            obj.name=obj.name.substr(1,obj.name.size()-2);
            break;
        case STATE_POS:
            in>>r;
            obj.task_state=util::state_map[r];
            break;
        case PPID_POS:
            in>>obj.ppid;
            break;
        case TIME_POS:
            in>>obj.utime>>obj.stime>>obj.cutime>>obj.cstime;
            i+=3;
            break;
        case PRIORITY_POS:
            in>>obj.priority;
            break;
        case THREAD_POS:
            in>>obj.thread_num;
            break;
        default:
            in>>str;
            break;
        }
    }
    return in;
}
double cpu_time_info::sum() const{
    return usr+nice+sys+idle+io_wait+irq+soft_irq+steal;
}
double cpu_time_info::non_idle() const{
    return usr+nice+sys+irq+soft_irq+steal;
}
unsigned long proc_cpu_info::time_sum() const{
    return (utime+stime+cutime+cstime)/(float)sysconf(_SC_CLK_TCK);
}

cpu::cpu():sys_rate(0),usr_rate(0),cpu_use_rate(0){
    ifstream type_file("/proc/cpuinfo");
    if(!type_file.is_open()){
        cout<<"error open cpu info file!"<<endl;
        return;
    }
    regex name_reg("model name\\s+:(.*)");
    regex num_reg("cpu cores\\s+:(.*)");
    regex freq_reg("cpu MHz\\s+:(.*)");
    regex proc_reg("processor\\s+:(.*)");
    smatch matchResult;
    string line;
    this->processor=0;
    while(getline(type_file,line)){
        if(regex_match(line,matchResult,proc_reg)){
           this->processor++;
        }
        else if(regex_match(line,matchResult,freq_reg)){
            this->freq=matchResult[1];
        }
        else if(regex_match(line,matchResult,name_reg)){
           this->model_name=matchResult[1];
        }
        else if(regex_match(line,matchResult,num_reg)){
            this->core_num=matchResult[1];
        }
    }

    type_file.close();
}

void cpu::get_snapshot(cpu_time_info &s){
    ifstream cpu_stat("/proc/stat");
    if(!cpu_stat.is_open()){
        cout<<"error open cpu stat file!"<<endl;
        return;
    }
    cpu_stat>>s;
    cpu_stat.close();
}
double cpu::get_cpu_use(const cpu_time_info &SnapShot1,const cpu_time_info&SnapShot2){
    cpu_use_rate=abs((SnapShot2.non_idle()-SnapShot1.non_idle())/(SnapShot1.sum()-SnapShot2.sum()))*100.0;
    return cpu_use_rate;
}
double cpu::get_usr_rate(const cpu_time_info &s1,const cpu_time_info&s2){
    usr_rate=abs((s2.usr-s1.usr)/(double)(s2.sum()-s1.sum()))*100.0;
    return usr_rate;
}
double cpu::get_sys_rate(const cpu_time_info &s1,const cpu_time_info&s2){
    sys_rate=abs((s2.sys-s1.sys)/(double)(s2.sum()-s1.sum()))*100.0;
    return sys_rate;
}

/*
 * 读取所有进程信息
 * 全部存入进程数组内部
 * 要计算占用率时需要建两个数组
 * 此时还缺一个占用率的信息，故不是完整信息，只能作为内部功能函数调用
 */
unsigned long cpu::get_proc_info(proc_cpu_info *procs){
    DIR *dp;
    char dir[6]="/proc";
    struct dirent *entry;
    struct stat statbuf;

    if(!(dp=opendir(dir))){
        cout<<"open proc failed!"<<endl;
        return 0;
    }
    unsigned long count=0;
    unsigned long t_count=0;
    while((entry=readdir(dp))){//一直将目录读取完
            lstat(entry->d_name,&statbuf);

            if(S_ISDIR(statbuf.st_mode)){
                if(entry->d_name[0]>='0'&&entry->d_name[0]<='9'){//是进程目录
                    char filename[30];
                    sprintf(filename,"proc/%s/stat",entry->d_name);
                    ifstream proc_file(filename);

                    if(!proc_file.is_open())
                        continue;

                    procs[count].user=util::file_user(&statbuf);
                    proc_file>>procs[count];
                    t_count+=procs[count].thread_num;
                    count++;

                    proc_file.close();
                }
            }
        }
        closedir(dp);
        this->proc_num=count;
        this->thread_num=t_count;

        return count;
}
/*
 * 计算占用率后获得完整信息
 * 通过指针返回数组
 * 直接被界面调用的函数
 */
void cpu::get_proc_full(proc_cpu_info *procs){
    proc_cpu_info procs1[PROC_MAX_NUM];
    proc_cpu_info procs2[PROC_MAX_NUM];
    cpu_time_info s1,s2;

    get_snapshot(s1);
    get_proc_info(procs1);
    usleep(200000);
    get_snapshot(s2);
    get_proc_info(procs2);
    for(int i=0;i<this->proc_num;i++){
        procs2[i].cpu_rate=(procs2[i].time_sum()-procs1[i].time_sum())/
                (s2.sum()-s1.sum());
    }
    return;
}
double cpu::calc_cpu_rate(const proc_cpu_info* before, const proc_cpu_info* after,const unsigned long long &cpuTime){
    cpu_time_info cur;
    get_snapshot(cur);
    double cpuTimeA =cur.sum()-cpuTime;
    unsigned long long processcpuTime = ((after->utime + after->stime)
                    - (before->utime + before->stime));

    return (processcpuTime / cpuTimeA) * 100.0;
}


