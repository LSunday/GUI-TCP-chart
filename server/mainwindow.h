#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTcpServer>
#include<QTcpSocket>
#include<QFile>

#define bufsize 1024*4


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void senduserdata();//发送文件数据
private slots:

    void socket_newconnect();//客户端连接槽函数

    void socket_senduserfile();//发送用户信息文件槽函数

    void on_pushButton_clicked();//侦听槽函数

private:
    Ui::MainWindow *ui;
    QTcpServer *server;     //服务器对象
    QTcpSocket *socket;     //与客户端连接socket
    QFile file;             //文件对象
    QString filename;       //文件名
    qint64 filesize;        //文件数据大小
    qint64 sendsize;        //已发送数据大小
};

#endif // MAINWINDOW_H
