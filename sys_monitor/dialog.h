#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    QString pass_args(QString str){return str;}
    ~Dialog();

private slots:
    void on_pushButton_clicked();
signals:
    void sendStr(QString);
private:
    Ui::Dialog *ui;

};

#endif // DIALOG_H
