#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<deque>
#include <QVector>
#include <QTime>
#include <QTimer>
#include<QAction>
#include <ctime>
#include <cstdlib>
#include"dialog.h"
#include "qcustomplot.h"
#include"cpu.h"
#include"memory.h"
#include"sys.h"
#include"tablenumberitem.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{

    Q_OBJECT
private:
    Ui::MainWindow *ui;
    Dialog *newDialog;
    cpu  cpu_info;
    memory  mem_info;
    sys sys_info;
    int selectedRowInfoID;
    storedProcType prevProcs;
    std::unordered_map<QString, QIcon> processIconCache;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    deque<double> cpu_sys;
    deque<double> cpu_usr;
    deque<double> mem_use;
    deque<double> mem_swapped;
    int minutes;
    int hours;
    int days;

    void init_cpu_widget();
    void init_mem_widget();
    void init_sys_messages();
    void Graph_Show(QCustomPlot *customPlot,int arg);
public slots:
    void update_curves();
    void loop();
    void updateTable();
    void cpu_select(QModelIndex current);
    void mem_select(QModelIndex current);
    void filterProcesses(QString filter);
    void update_time_info();
    void update_sys_info();
    void shutdown();
    void reboot();
signals:

    void update_sys_data();
    void updateTableData();
    void signalFilterProcesses(QString filter);
private slots:
    void on_kill_button_clicked();
    void on_new_proc_clicked();
    void new_proc(QString);
};

#endif // MAINWINDOW_H
