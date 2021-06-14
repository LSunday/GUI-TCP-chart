#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include<QMessageBox>
#include<QString>
#include <QFileDialog>
#include <QFileInfo>
#include <QByteArray>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server=new QTcpServer();//初始化服务器对象

    //设置IP地址和端口
    ui->lineEdit_IP->setText("127.0.0.1");
    ui->lineEdit_Port->setText("6666");

     //新客户端连接信号触发socket_newconnect槽函数
    connect(server,&QTcpServer::newConnection,this,&MainWindow::socket_newconnect);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::socket_newconnect()
{
    //从工作路径文件夹userdata获得用户信息文件
    QString currentpath=QDir::currentPath();
    QString  filepath=QString("%1/userdata/用户信息.csv").arg(currentpath);

    if(filepath.isEmpty()==false)
    {
        filename.clear();
        filesize = 0;
        //获取文件信息：名字、大小
        QFileInfo fileinfo(filepath);
        filename = fileinfo.fileName();
        filesize = fileinfo.size();
        sendsize = 0;   //已经发送文件大小
        //以只读方式打开文件
        file.setFileName(filepath);
        if(file.open(QIODevice::ReadOnly)==false)
        {
            qDebug()<<"文件打开失败";
        }
    }
    else
    {
         qDebug()<<"路径无效";
    }
    //客户端连接
   socket=server->nextPendingConnection();

   //发送头部信息
   QString filehead=QString("%1,%2").arg(filename).arg(filesize);
   qint64 writeresult  = socket->write(filehead.toUtf8());
   if(writeresult < 0){
       file.close();
   }
   //收到客户端信息后，发送用户信息文件
   connect(socket,&QTcpSocket::readyRead,this,&MainWindow::socket_senduserfile);
}
void MainWindow::socket_senduserfile()
{
    QByteArray buf = socket->readAll();
    if(QString(buf)=="fileheadrecv")
    {
        senduserdata();
    }
    else if(QString(buf)=="filewritedone")
    {
        socket->close();
        file.close();
    }
}
void MainWindow::senduserdata()
{
    qint64 len = 0;
    //分批发送文件数据循环，一次发送4KB
    do{
        char buf[bufsize] = {0};//发送缓冲区初始化
        len = 0;
        len = file.read(buf,bufsize);  //len为读取的字节数
        len = socket->write(buf,len);    //len为发送的字节数
        sendsize =sendsize+len;//已发数据大小
       qDebug()<<len;
    }while(len > 0);
}
void MainWindow::on_pushButton_clicked()
{
    //侦听槽函数
    QHostAddress IP(ui->lineEdit_IP->text());
    quint16 port=ui->lineEdit_Port->text().toUInt();
    if(!server->listen(IP,port))
    {
        QMessageBox::warning(this,"warning",tr("侦听失败"),QMessageBox::Yes);
    }
    else
    {
        QMessageBox::warning(this,"warning",tr("开始侦听"),QMessageBox::Yes);
    }

}
