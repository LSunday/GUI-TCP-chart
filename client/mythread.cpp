#include "mythread.h"
#include"mainwindow.h"
#include<QString>

mythread::mythread(QObject *parent) : QThread(parent)
{
   linetable = new QLineEdit;//初始化LineEdit
   sontable=new QTableWidget;//初始化tablewidget
}
 void mythread::run()
{
     QString ID=linetable->text();//获取用户信息
     //检索用户信息，获取已登录用户所在tablewidget行号
     for(s=0;s<row-1;s++)
     {
         if(sontable->item(s,0)->text()==ID)
         {
            break;
         }
     }

    qDebug()<<ID;
    qDebug()<<s;

    emit isdone();//发射进程结束信号

}
