
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<sys/types.h>
#include <signal.h>

#define HEAD_NUM 7
#define PID_COL 5
#define INTERVAL 1.0

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    selectedRowInfoID(0)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    timer->start(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(update_time_info()));
    connect(timer,SIGNAL(timeout()),this,SLOT(loop()));
    connect(timer,SIGNAL(timeout()),this,SLOT(update_curves()));
    connect(ui->actionshutdown,&QAction::triggered,this,&MainWindow::shutdown);
    connect(ui->actionreboot,&QAction::triggered,this,&MainWindow::reboot);

    // set a function to run when a row is selected
    connect(ui->cpu_widget->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this,SLOT(cpu_select(QModelIndex)));
    connect(ui->mem_widget->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this,SLOT(mem_select(QModelIndex)));
    connect(ui->search_edit,SIGNAL(textChanged(QString)),this,SLOT(filterProcesses(QString)));
    connect(this,SIGNAL(signalFilterProcesses(QString)),this,SLOT(filterProcesses(QString)));
    connect(this,SIGNAL(updateTableData()),SLOT(updateTable()));
    connect(this,SIGNAL(update_sys_data()),SLOT(update_sys_info()));

    init_cpu_widget();
    init_mem_widget();
    init_sys_messages();

}
void MainWindow::reboot(){
    system("reboot");
}
void MainWindow::shutdown(){
    system("halt");
}
void MainWindow::init_sys_messages(){
    ui->host_name->setText(QString::fromStdString(sys_info.get_host()));
    ui->version->setText(QString::fromStdString(sys_info.get_version()));
    ui->core_num->setText(QString::fromStdString(cpu_info.get_core_num()));
    ui->processors->setText(QString::number(cpu_info.get_processor()));
    ui->CPU_freq->setText(QString::fromStdString(cpu_info.get_frequency()));
    ui->model_name->setText(QString::fromStdString(cpu_info.get_cpu_name()));
}
void MainWindow::update_time_info(){
    double acc_time=sys_info.get_time_info();

    int day=int(acc_time/(3600*24));
    int hour=int(acc_time/3600)-day*24;
    int minutes=int(acc_time/60)-day*24*60-hour*60;
    int seconds=int(acc_time)%60;

    //QString::number(acc_time,'f',10)
    QString time_info=QString::number(day)+"d "+QString::number(hour)+"h "+QString::number(minutes)+"m "+QString::number(seconds)+"s ";
    ui->run_time->setText(time_info);
    ui->start_time->setText(QString::fromStdString(sys_info.get_start_time()));
    ui->cur_time->setText(QString::fromStdString(sys_info.get_cur_time()));
}
/**
 * @brief processInformationWorker::filterProcesses Hide all processes which do not match the search term
 * @param filter The search term to use
 */
void MainWindow::filterProcesses(QString filter)
{
    //not sensible of the letter
    filter = filter.toLower();

    for(int i = 0; i < ui->cpu_widget->rowCount(); i++)
    {
        // check if the row is already hidden, if so, skip it
        /// TODO: find a better fix for this, can't check using QTableWidget
        /// so just check the PID manually

            QTableWidgetItem* name = ui->cpu_widget->item(i,0); // process name
            QTableWidgetItem* pid = ui->cpu_widget->item(i, PID_COL);
            if(name!=NULL&&pid!=NULL){
                bool nameContains = name->text().toLower().contains(filter);
                bool pidContains = pid->text().contains(filter);
                ui->cpu_widget->setRowHidden(i, (!nameContains && !pidContains));
            }
    }
    for(int i = 0; i < ui->mem_widget->rowCount(); i++)
    {
        // check if the row is already hidden, if so, skip it
        /// TODO: find a better fix for this, can't check using QTableWidget
        /// so just check the PID manually

            QTableWidgetItem* name = ui->mem_widget->item(i,0); // process name
            QTableWidgetItem* pid = ui->mem_widget->item(i, PID_COL);
            if(name!=NULL&&pid!=NULL){
                bool nameContains = name->text().toLower().contains(filter);
                bool cmdLineContains = util::getProcessCmdline(pid->text().toInt()).toLower().contains(filter);
                ui->mem_widget->setRowHidden(i, (!nameContains && !cmdLineContains));
            }
    }

}

void MainWindow::cpu_select(QModelIndex current)
{
    // remember the tid so we can get the correct row again when we update the table
    if(ui->cpu_widget->item(current.row(),PID_COL)!=NULL)
        selectedRowInfoID = ui->cpu_widget->item(current.row(),PID_COL)->text().toInt();
}
void MainWindow::mem_select(QModelIndex current){
    // remember the tid so we can get the correct row again when we update the table
    if(ui->mem_widget->item(current.row(),PID_COL)!=NULL)
        selectedRowInfoID = ui->mem_widget->item(current.row(),PID_COL)->text().toInt();
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::init_cpu_widget(){
    QStringList headers = {"进程名称","%CPU","CPU时间","线程","状态","PID","用户","priority"};
    ui->cpu_widget->setColumnCount(8);
    ui->cpu_widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->cpu_widget->setHorizontalHeaderLabels(headers);
    ui->cpu_widget->verticalHeader()->setVisible(false);
    ui->cpu_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->cpu_widget->resizeColumnsToContents();

}
void MainWindow::init_mem_widget(){
    QStringList headers = {"进程名称","虚拟内存","物理内存","%MEM","线程","PID","用户"};
    ui->mem_widget->setColumnCount(7);
    ui->mem_widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->mem_widget->setHorizontalHeaderLabels(headers);
    ui->mem_widget->verticalHeader()->setVisible(false);
    ui->mem_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->mem_widget->resizeColumnsToContents();
}
void MainWindow::loop(){
    emit(update_sys_data());
    emit(updateTableData());
}
void MainWindow::update_sys_info(){
    cpu_time_info s;
    cpu_info.get_snapshot(s);

    ui->sys_cpu->setText(QString::number(cpu_info.get_sys_rate(cpu_info.cpu_time,s),'f',3));
    ui->usr_cpu->setText(QString::number(cpu_info.get_usr_rate(cpu_info.cpu_time,s),'f',3));
    ui->nonidle_cpu->setText(QString::number(cpu_info.get_cpu_use(cpu_info.cpu_time,s),'f',3));
    ui->thread_num->setText(QString::number(cpu_info.get_total_thread()));
    ui->proc_num->setText(QString::number(cpu_info.get_total_procs()));

    mem_info.get_mem_info();
    ui->mem_total->setText(QString::number(mem_info.get_mem_total()));
    ui->mem_use->setText(QString::number(mem_info.get_mem_used()));
    ui->buffer->setText(QString::number(mem_info.get_buffer()));
    ui->swap->setText(QString::number(mem_info.get_swapped()));

}
void MainWindow::updateTable(){
        /*--------------------sysinfo---------------*/
        cpu_time_info cur_time;
        cpu_info.get_snapshot(cur_time);
        double cpu_rate= cpu_info.get_cpu_use(cpu_info.cpu_time,cur_time);
        ui->cpu_rate->setText(QString::number(cpu_rate,'f',3));

        if (cpu_info.cpu_time.sum()!=0) {

            float sys_rate = cpu_info.get_sys_rate(cpu_info.cpu_time,cur_time);
            float usr_rate = cpu_info.get_usr_rate(cpu_info.cpu_time,cur_time);
            if(cpu_sys.size()==100){
                cpu_sys.pop_front();
            }
            if(cpu_usr.size()==100){
                cpu_usr.pop_front();
            }

            cpu_sys.push_back(sys_rate);
            cpu_usr.push_back(usr_rate);
        }

        mem_info.get_mem_info();
        ui->mem_rate->setText(QString::number(mem_info.get_use_rate(),'f',3));

        float mem_rate=mem_info.get_use_rate();
        float swap_rate=mem_info.get_swap_rate();
        if(mem_rate>0){
            if(mem_use.size()==100){
                mem_use.pop_front();
            }
            if(mem_swapped.size()==100){
                mem_swapped.pop_front();
            }
            mem_use.push_back(mem_rate);
            mem_swapped.push_back(swap_rate);
        }


        /*-------------------process-----------------*/
        storedProcType processes;
        proc_cpu_info procs[PROC_MAX_NUM];
        unsigned long proc_num=0;
        proc_num=cpu_info.get_proc_info(procs);
        for(int i=0;i<proc_num;i++) {
            processes[procs[i].pid]=procs[i];
        }

        // fill in cpu%
        if (prevProcs.size()>0) {
            // we have previous proc info
            for(auto &newItr:processes) {
                auto prevItr = prevProcs[newItr.first];
                newItr.second.cpu_rate = cpu_info.calc_cpu_rate(&prevItr, &newItr.second,cpu_info.get_total_time());
            }
        }

       // update the cpu info for next loop
        cpu_info.get_snapshot(cpu_info.cpu_time);
        ui->cpu_widget->setUpdatesEnabled(false); // avoid inconsistant data
        ui->cpu_widget->setSortingEnabled(false);
        ui->cpu_widget->setRowCount(proc_num);


       unsigned int index = 0;
       for(auto &i:processes) {
           proc_cpu_info* p = (&i.second);
           QString processName = QString::fromStdString(p->name);
           QTableWidgetItem* processNameTableItem = new QTableWidgetItem(processName);
           processNameTableItem->setToolTip(util::getProcessCmdline(p->pid));
           processNameTableItem->setIcon(util::getProcessIconFromName(processName,processIconCache));
           ui->cpu_widget->setItem(index,0,processNameTableItem);

           QString cpu = QString::number(p->cpu_rate,'f',3);

           ui->cpu_widget->setItem(index,1,new TableNumberItem(cpu));
           unsigned long cpu_time=p->time_sum();
           QString proc_time=QString::number(cpu_time/60)+":"+QString::number(cpu_time%60);
           ui->cpu_widget->setItem(index,2,new TableNumberItem(proc_time));

           QString thread_num=QString::number(p->thread_num);
           ui->cpu_widget->setItem(index,3,new TableNumberItem(thread_num));

           QString state = QString::fromStdString(p->task_state);
           ui->cpu_widget->setItem(index,4,new QTableWidgetItem(state));

           QString id = QString::number(p->pid);
           ui->cpu_widget->setItem(index,5,new TableNumberItem(id));

           QString user = QString::fromStdString(p->user);
           ui->cpu_widget->setItem(index,6,new QTableWidgetItem(user));

           QString priority = QString::fromStdString(p->priority);
           ui->cpu_widget->setItem(index,7,new QTableWidgetItem(priority));

           ui->cpu_widget->showRow(index);

           // update the row selection to reflect the row that was previously selected
           if (selectedRowInfoID>0) {
               if (selectedRowInfoID == p->pid) {
                   // this is the same process
                   ui->cpu_widget->selectRow(index);
               }
           }
           index++;
       }

       ui->cpu_widget->setUpdatesEnabled(true);
       ui->cpu_widget->setSortingEnabled(true);


        ui->mem_widget->setUpdatesEnabled(false); // avoid inconsistant data
        ui->mem_widget->setSortingEnabled(false);
        ui->mem_widget->setRowCount(proc_num);
       proc_mem_info procs_mem[PROC_MAX_NUM];
       proc_num=mem_info.get_proc_info(procs_mem);

       index = 0;
       for(int i=0;i<proc_num;i++) {
           proc_mem_info* p = &procs_mem[i];
           QString processName = QString::fromStdString(p->name);
           QTableWidgetItem* processNameTableItem = new QTableWidgetItem(processName);
           processNameTableItem->setToolTip(util::getProcessCmdline(p->pid));
           processNameTableItem->setIcon(util::getProcessIconFromName(processName,processIconCache));
           ui->mem_widget->setItem(index,0,processNameTableItem);

           QString size = QString::number(p->size);
           ui->mem_widget->setItem(index,1,new TableNumberItem(size));
           QString rss=QString::number(p->rss);
           ui->mem_widget->setItem(index,2,new TableNumberItem(rss));

           QString mem_rate=QString::number(p->mem_rate);
           ui->mem_widget->setItem(index,3,new TableNumberItem(mem_rate));

           QString thread_num = QString::fromStdString(p->thread_num);
           ui->mem_widget->setItem(index,4,new QTableWidgetItem(thread_num));

           QString id = QString::number(p->pid);
           ui->mem_widget->setItem(index,5,new TableNumberItem(id));

           QString user = QString::fromStdString(p->user);
           ui->mem_widget->setItem(index,6,new QTableWidgetItem(user));

           ui->mem_widget->showRow(index);

           // update the row selection to reflect the row that was previously selected
           if (selectedRowInfoID>0) {
               if (selectedRowInfoID == p->pid) {
                   // this is the same process
                   ui->mem_widget->selectRow(index);
               }
           }
           index++;
       }

       ui->mem_widget->setUpdatesEnabled(true);
       ui->mem_widget->setSortingEnabled(true);

       emit(signalFilterProcesses(ui->search_edit->text()));

       // keep processes we've read for cpu calculations next cycle
       prevProcs = processes;
}

void MainWindow::update_curves()
{
    Graph_Show(ui->cpu_curve,1);
    Graph_Show(ui->mem_curve,2);
}

void MainWindow::Graph_Show(QCustomPlot *CustomPlot,int arg)
{

    //设置画布颜色
     QLinearGradient plotGradient;
     plotGradient.setColorAt(0, QColor(80, 80, 80));
    CustomPlot->setBackground(plotGradient);

   //设置坐标颜色/坐标名称颜色
    CustomPlot->xAxis->setLabelColor(Qt::white);//文字颜色
    CustomPlot->yAxis->setLabelColor(Qt::white);
    CustomPlot->xAxis->setTickLabelColor(Qt::white);//坐标轴数字颜色
    CustomPlot->yAxis->setTickLabelColor(Qt::white);
    CustomPlot->xAxis->setBasePen(QPen(Qt::white, 1));//坐标轴颜色及宽度
    CustomPlot->yAxis->setBasePen(QPen(Qt::white, 1));
    CustomPlot->xAxis->setTickPen(QPen(Qt::white, 1));//主刻度
    CustomPlot->yAxis->setTickPen(QPen(Qt::white, 1));
    CustomPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));//副刻度
    CustomPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));

    //设置属性可缩放，移动等
    CustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                         QCP::iSelectLegend | QCP::iSelectPlottables);


    QVector<double> time(100);
    QVector<double> y_sys(100);
    QVector<double> y_usr(100);
    QVector<double> y_mem(100);
    QVector<double> y_swap(100);
    for(int i=0;i<cpu_sys.size();i++)
    {
        time[i] = i;
        y_sys[i]=cpu_sys[i];
        y_usr[i]=cpu_usr[i];
    }
    for(int i=0;i<mem_use.size();i++){
        y_mem[i]=mem_use[i];
        y_swap[i]=mem_swapped[i];
    }


    if(arg==1){
        CustomPlot->addGraph();//添加一条曲线
        CustomPlot->graph(0)->setPen(QPen(Qt::red)); //0是曲线序号，添加的第一条是0，设置曲线颜色

        CustomPlot->graph(0)->setData(time,y_sys); //输出各点的图像，x和y都是QVector类

        CustomPlot->xAxis->setLabel("time");//x轴的文字
        CustomPlot->yAxis->setLabel("rate");//y轴的文字

        CustomPlot->xAxis->setRange(0,100);//x轴范围
        CustomPlot->yAxis->setRange(0,100);//y轴范围

        CustomPlot->addGraph();//添加第二条曲线
        CustomPlot->graph(1)->setPen(QPen(Qt::blue)); //1是曲线序号，添加的第二条是1，设置曲线颜色
        CustomPlot->graph(1)->setData(time,y_usr);
        CustomPlot->replot();//重绘
    }
    else{
       CustomPlot->addGraph();

       CustomPlot->graph(0)->setPen(QPen(Qt::green));
       CustomPlot->graph(0)->setData(time,y_mem);

       CustomPlot->xAxis->setLabel("time");//x轴的文字
       CustomPlot->yAxis->setLabel("rate");//y轴的文字

       CustomPlot->xAxis->setRange(0,100);//x轴范围
       CustomPlot->yAxis->setRange(0,100);//y轴范围

       CustomPlot->addGraph();//添加第二条曲线
       CustomPlot->graph(1)->setPen(QPen(Qt::blue)); //1是曲线序号，添加的第二条是1，设置曲线颜色
       CustomPlot->graph(1)->setData(time,y_swap);
       CustomPlot->replot();//重绘

    }
}

void MainWindow::on_kill_button_clicked()
{
    // get PID
    int pid =selectedRowInfoID;

    QMessageBox::StandardButton reply;
    std::string info = "Are you sure you want to kill the process?";
    reply = QMessageBox::question(ui->tabWidget,"Info", QString::fromStdString(info),
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // attempt to send signal
        if (kill(pid,SIGKILL)!=0) {
            if (errno==EPERM) {
                // show a dialogue that we did not have permission to SIGKILL pid
                QMessageBox::information(ui->tabWidget, "Error",
                                "Could not send signal, permission denied!", QMessageBox::Ok);
            }
        }
    }
}

void MainWindow::new_proc(QString newstr)
{
    QFileInfo file_info(newstr);
    if(file_info.isFile())
    {
        QProcess *proc=new QProcess;
        proc->start(newstr);
    }
    else{
        QMessageBox::information(NULL, "Warning", "error path!Please input again", QMessageBox::Yes, QMessageBox::Yes);
    }

}
void MainWindow::on_new_proc_clicked()
{
    newDialog = new Dialog();
    newDialog->setModal(true); //模态
    QObject::connect(newDialog,SIGNAL(sendStr(QString)),this,SLOT(new_proc(QString)));

    newDialog->show();

}
