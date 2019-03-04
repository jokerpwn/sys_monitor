#include "memory.h"
#include<cstdlib>
memory::memory():mem_total(0),mem_free(0),buffers(0),
    use_rate(0),swapped(0),proc_num(0)
{

}
istream& operator >>(istream& in,proc_mem_info& obj){
    regex name_reg("Name:\\s+(.*)");
    regex pid_reg("Pid:\\s+(.*)");
    regex rss_reg("VmRSS:\\s+(\\d+).*");
    regex size_reg("VmSize:\\s+(\\d+).*");
    regex thread_reg("Threads:\\s+(.*)");
    string line;
    string str;
    smatch matchResult;
    while(getline(in,line)){
        if(regex_match(line,matchResult,name_reg)){
            obj.name=matchResult[1];
        }
        else if(regex_match(line,matchResult,pid_reg)){
            str=matchResult[1];
            obj.pid=atoi(str.c_str());
        }
        else if(regex_match(line,matchResult,rss_reg)){
            obj.rss=strtol(string(matchResult[1]).c_str(),NULL,0);
        }
        else if(regex_match(line,matchResult,size_reg)){
            obj.size=strtol(string(matchResult[1]).c_str(),NULL,0);
        }
        else if(regex_match(line,matchResult,thread_reg)){
            obj.thread_num=matchResult[1];
        }
    }

    return in;
}
/*
 * 输入：null
 * 输出：进程内存结构信息指针（数组）
 * 方法：通过读取/proc目录下所有数字符号的进程目录+status文件
 *      正则匹配所需的信息，需要计算的内容要进行字符串转数字的操作
 */
unsigned long memory::get_proc_info(proc_mem_info *procs){
    DIR *dp;
    char dir[10]="/proc";
    struct dirent *entry;
    struct stat statbuf;

    if(!(dp=opendir(dir))){
        cout<<"open proc failed!"<<endl;
        return 0;
    }
    unsigned long count=0;
    while((entry=readdir(dp))){//一直将目录读取完
            lstat(entry->d_name,&statbuf);

            if(S_ISDIR(statbuf.st_mode)){
                if(entry->d_name[0]>='0'&&entry->d_name[0]<='9'){//是进程目录
                    char filename[30];
                    sprintf(filename,"proc/%s/status",entry->d_name);
                    ifstream proc_file(filename);

                    if(!proc_file.is_open())
                        continue;
                    procs[count].user=util::file_user(&statbuf);
                    proc_file>>procs[count];
                    procs[count].mem_rate=(procs[count].rss/(float)(mem_total-mem_free))*100.0;
                    count++;
                    proc_file.close();
                }
            }
        }
        chdir("../");
        closedir(dp);
        this->proc_num=count;
        return count;
}
void memory::get_mem_info(){
    ifstream info_file("/proc/meminfo");
    if(!info_file.is_open()){
        cout<<"error open mem info file!"<<endl;
        return;
    }
    regex size_reg("MemTotal:\\s+(\\d+).*");
    regex free_reg("MemFree:\\s+(\\d+).*");
    regex buffer_reg("Buffers:\\s+(\\d+).*");
    regex swap_total_reg("SwapTotal:\\s+(\\d+).*");
    regex swap_free_reg("SwapFree:\\s+(\\d+).*");
    smatch matchResult;
    string line;
    while(getline(info_file,line)){
        if(regex_match(line,matchResult,size_reg)){
            this->mem_total=strtol(string(matchResult[1]).c_str(),NULL,0);
        }
        else if(regex_match(line,matchResult,free_reg)){
            this->mem_free=strtol(string(matchResult[1]).c_str(),NULL,0);
        }
        else if(regex_match(line,matchResult,buffer_reg)){
           this->buffers=strtol(string(matchResult[1]).c_str(),NULL,0);
        }
        else if(regex_match(line,matchResult,swap_total_reg)){
            unsigned long total=strtol(string(matchResult[1]).c_str(),NULL,0);
            getline(info_file,line);
            regex_match(line,matchResult,swap_free_reg);
            unsigned long free=strtol(string(matchResult[1]).c_str(),NULL,0);
            this->swapped=total-free;
        }
    }
    this->use_rate=((float)(mem_total-mem_free)/mem_total)*100.0;
    this->swap_rate=((float)swapped/mem_total)*100.0;
    info_file.close();
}
