#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<deque>
#include <QVector>
#include <QTime>
#include <QTimer>
#include <ctime>

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
signals:

    void update_sys_data();
    void updateTableData();
    void signalFilterProcesses(QString filter);
private slots:
    void on_kill_button_clicked();
};

#endif // MAINWINDOW_H
