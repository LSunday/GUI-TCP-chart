#ifndef MYTHREAD_H
#define MYTHREAD_H

#include<QThread>//线程操作头文件
#include<QDebug>
#include<QTableWidget>
#include<QLineEdit>

class mythread : public QThread
{
    Q_OBJECT
public:
    explicit mythread(QObject *parent = nullptr);

protected:
    void run();//重载线程运行函数

signals:
    void isdone();//线程运行结束标志

public:
    QTableWidget *sontable;//tablewidget类，储存用户信息
    QLineEdit *linetable;//lineEdit类，用来储存已登录用户信息

};

#endif // MYTHREAD_H
