 #include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include<QDebug>
#include<QHostAddress>
#include<QString>
#include<QFile>
#include<QFileDialog>
#include<QTextStream>

int s=0;
int row;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //作为客户端从服务器获取用户信息
    socket=new QTcpSocket();//初始化socket
    //收到数据信号后，转到socket_readuserdata槽函数
    connect(socket,&QTcpSocket::readyRead,this,&MainWindow::socket_readuserdata);
    //与用户信息服务器断开连接以后，转到socket_disconnected槽函数
    connect(socket,&QTcpSocket::disconnected,this,&MainWindow::socket_disconnected);

    //预填入IP地址和端口号
    ui->lineEdit_IP->setText("127.0.0.1");
    ui->lineEdit_Port->setText("6666");

    //设置用户信息表
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置禁止编辑表格
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//设置行选中
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//设置滚动条

    //用户查找次线程
    thread=new mythread(this);//初始化次线程对象
    thread->sontable=ui->tableWidget;//将tablewidget传递进入线程类
    thread->linetable=ui->lineEdit_ID;//将LineEdit传递进入线程类
    //线程结束后，触发线程结束槽函数
    connect(thread,&mythread::isdone,this,&MainWindow::threadisdone);

   //作为服务器初始化服务器对象
    server=new QTcpServer();//初始化服务器对象
    socket_server =new QTcpSocket;//初始化socket
    //聊天时作为服务器，有客户端连接时，触发servernewconnect槽函数
    connect(server,&QTcpServer::newConnection,this,&MainWindow::servernewconnect);

    //作为客户端初始化客户端对象
    socket_client =new QTcpSocket;//初始化socket
    //聊天时作为客户端，收到数据时，触发socket_clientreadydata槽函数
    connect(socket_client,&QTcpSocket::readyRead,this,&MainWindow::socket_clientreadydata);

    //初始化时间对象
    currenttime=QDateTime::currentDateTime();//初始化时间对象，获得当前时间

 }

MainWindow::~MainWindow()
{
    delete ui;
}

//登录按钮槽函数
void MainWindow::on_pushButton_connect_clicked()
{
    socket->abort();//重置套接字
    socket->connectToHost(ui->lineEdit_IP->text(),ui->lineEdit_Port->text().toInt());//连接用户信息服务器
    if(socket->waitForConnected(3000))
    {
     //等待套接字连接
        qDebug()<<"连接用户信息服务器成功";
    }
}

void MainWindow::socket_readuserdata()
{
    //接收头部信息
    QByteArray buf = socket->readAll();
    if(isstart==true)
    {
        isstart = false;
        filename = QString(buf).section(",",0,0);
        filesize = QString(buf).section(",",1,1).toInt();
        recvsize = 0;
        file.setFileName(filename);
        if(file.open(QIODevice::WriteOnly)==false)
        {
          QMessageBox::warning(this,"warning",tr("文件打开失败"),QMessageBox::Yes);
        }
        socket->write("fileheadrecv");
    }
    else
    {
        qDebug()<<"开始接收文件";
        //接收用户信息文件文件
        qint64 len = file.write(buf);
        recvsize=recvsize+len;
        if(recvsize == filesize)
        {
            //接收完毕
            file.close();
          qDebug()<<"接受文件成功";
            socket->write("filewritedone");
            socket->close();
         }
    }
}
void MainWindow::socket_disconnected()
{
   //将用户信息文件写入tablewidget
   ui->tableWidget->setColumnCount(2);//设置tablewidget的列数
   //从工作路径获取用户信息文件
   QString currentpath=QDir::currentPath();
   QString  path=QString("%1/用户信息.csv").arg(currentpath);
   QFile file(path);
   //以只读方式打开，以文本方式打开文件
   file.open(QIODevice::ReadOnly | QIODevice::Text);
   //写入数据
   QTextStream filestream(&file);
   QString line;
   QStringList filerow;
   row = 1;
   while (!filestream.atEnd())
   {
       line =filestream.readLine();
       filerow = line.split(",");//按照","分割字符，因为csv文件是以逗号分隔数据的
       for (int c = 0; c <filerow.size(); c++)
       {
       ui->tableWidget->setRowCount(row);//设置行数
       ui->tableWidget->setItem(row - 1, c, new QTableWidgetItem(filerow[c]));//显示数据
       }
       row++;
   }
   //开始用户检索线程
    thread->start();
}
void MainWindow::threadisdone()
{
   //退出线程，阻塞线程
   thread->quit();
   thread->wait();
   //判断用户信息是否输入正确
   if(s==row-1)
   {
        QMessageBox::warning(this,"warning",tr("用户名错误"),QMessageBox::Yes);
        this->close();//关闭窗口
   }
   else
   {
        //作为服务器开始侦听
        QHostAddress IP(ui->tableWidget->item(s,1)->text());
        quint16 Port=6666;
        if(server->listen(IP,Port))
        {
            QMessageBox::warning(this,tr("连接提示"),tr("登录成功,开始侦听"),QMessageBox::Yes);
        }
   }
}

void MainWindow::on_tableWidget_clicked(const QModelIndex &index)
{
    //与点击的用户建立TCP连接
    socket_client->abort();
    int c=ui->tableWidget->currentRow();
    QHostAddress IP(ui->tableWidget->item(c,1)->text());
    socket_client->connectToHost(IP,6666);
    if(socket_client->waitForConnected(3000))
    {
        QMessageBox::warning(this,tr("连接提示"),tr("连接客户端成功"),QMessageBox::Yes);
    }
}
void MainWindow::socket_clientreadydata()
{
    //作为客户端收到信息并转码为UTF—8并加入时间与对端地址信息写入聊天框
    QByteArray tempread ;
    tempread=socket_client->readAll();
    QTextCodec *tr=QTextCodec::codecForName("UTF-8");
    QString serverIP=socket_client->peerAddress().toString();
    QString tempread1=QString("[%1]%2：%3").arg(serverIP).arg(currenttime.toString("yyyy-MM-dd hh:mm:ss")).arg(tr->toUnicode(tempread));
    ui->listWidget_recieve->addItem(new QListWidgetItem(tempread1));
}

void MainWindow::on_pushButton_send_clicked()
{
    //发送信息
    if(socket_client)
    {
        qint64 writeresult = socket_client->write(ui->textEdit->toPlainText().toUtf8(),(ui->textEdit->toPlainText().toUtf8().length()));
        bool boolflush=socket_client->flush();
        if(writeresult!=-1&&boolflush==1)
        {
            if(writeresult==0)
            {
                QMessageBox::warning(this,"warning",tr("数据结果返回0"),QMessageBox::Yes);
            }
        }
            qDebug()<<"成功";
    }
    if(socket_server)
    {
        qint64 writeresult = socket_server->write(ui->textEdit->toPlainText().toUtf8(),(ui->textEdit->toPlainText().toUtf8().length()));
        bool boolflush=socket_server->flush();
        if(writeresult!=-1&&boolflush==1)
        {
            if(writeresult==0)
            {
                QMessageBox::warning(this,"warning",tr("数据结果返回0"),QMessageBox::Yes);
            }
        }


            qDebug()<<"成功";
    }
    ui->textEdit->clear();

}
void MainWindow::servernewconnect()
{
    socket_server=server->nextPendingConnection();
    //作为服务器接收到收到数据信号，触发槽函数socket_serverreadydata
    connect(socket_server,&QTcpSocket::readyRead,this,&MainWindow::socket_serverreadydata);
}
void MainWindow::socket_serverreadydata()
{
     //作为服务器收到信息并转码为UTF—8并加入时间与对端地址信息写入聊天框
    QByteArray tempread ;
    tempread=socket_server->readAll();
    QTextCodec *tr=QTextCodec::codecForName("UTF-8");
    QString clientIP=socket_server->peerAddress().toString();
    qDebug()<<clientIP;
    QString tempread1=QString("[%1]%2：%3").arg(clientIP).arg(currenttime.toString("yyyy-MM-dd hh:mm:ss")).arg(tr->toUnicode(tempread));
    ui->listWidget_recieve->addItem(new QListWidgetItem(tempread1));
}
