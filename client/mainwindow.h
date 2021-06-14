#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTextCodec>//文本编码转换头文件
#include<QTcpServer>
#include<QDateTime>//时间头文件
#include<QFile>

//线程头文件
#include"mythread.h"

extern int s;//用户检索全局变量
extern int row;//用户检索全局变量

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

   void socket_disconnected();//作为客户端获取用户信息连接断开槽函数

   void on_pushButton_connect_clicked();//登陆按键槽函数

   void socket_readuserdata();//获取用户信息槽函数

   void threadisdone();//结束线程槽函数

   void on_tableWidget_clicked(const QModelIndex &index);//tablewidget用户选择槽函数

   void socket_clientreadydata();//作为客户端读取信息槽函数

   void on_pushButton_send_clicked();//发送键发送信息槽函数

   void servernewconnect();//作为服务器，新客户端连接槽函数

   void socket_serverreadydata();//作为服务器读取信息槽函数

private:
    Ui::MainWindow *ui;

    QTcpSocket *socket;//作为客户端获取用户信息socket
    QTcpServer *server;//作为服务器聊天服务器对象
    QTcpSocket *socket_server;//作为服务器聊天socket
    QTcpSocket *socket_client;//作为客户端聊天socket

    mythread *thread;//多线程对象

    QDateTime currenttime;//时间对象

    QFile file;             //文件对象
    QString filename;       //文件名
    qint64 filesize;        //文件数据大小
    qint64 recvsize;        //已接受数据大小
    bool isstart;           //头部开始接收标志位
};

#endif // MAINWINDOW_H
