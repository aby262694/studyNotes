#include "client.h"
#include "ui_client.h"

#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QMessageBox>

client::client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::client)
{
    ui->setupUi(this);

    //创建socket
    tcpsocket = new QTcpSocket(this);
    //未连接状态
    connectState = false;
    //接收信息
    connect(tcpsocket, &QTcpSocket::readyRead, this, &client::readMessage);
    //打印断开连接信息
    connect(tcpsocket, &QTcpSocket::disconnected, this, &client::disconnectSlot);

}

client::~client()
{
    delete ui;
}
//接收信息
void client::readMessage()
{
    QByteArray arr = tcpsocket->readAll();
    QString str;
    str = QDateTime::currentDateTime().toString("dddd yyyy.MM.dd hh:mm:ss") + '\n' + arr.data();
    //显示信息
    ui->textShowEdit->append(str);
}

//打印连接断开信息
void client::disconnectSlot()
{
    ui->textShowClientConnect->append("clint disconnected");
}

//连接槽函数
void client::on_buttonStart_clicked()
{
    //界面显示的地址
    QString ipStr = ui->lineEditIp->text();
    //界面显示的当前端口
    quint16 currentPort = ui->lineEditPort->text().toInt();
    if(!connectState)    //客户端还未连接服务端
    {
        //连接服务端
        tcpsocket->connectToHost(ipStr, currentPort);

        if(tcpsocket->waitForConnected())
        {
            //连接成功
            ui->textShowClientConnect->append("连接成功");
            ui->buttonStart->setText("客户端正在运行");
            ui->buttonStart->setEnabled(false);
            connectState = true;
        }
        else//连接失败
        {
            //连接错误信息提醒
            QMessageBox::warning(this, "连接失败", tcpsocket->errorString());
        }
    }
    else//客户端已经连接
    {
        ui->buttonStart->setEnabled(false);
        ui->buttonStart->setText("客户端正在运行");
    }
}
//发送槽函数
void client::on_buttonSend_clicked()
{
    QString str = ui->lineEditInput->text();
    //判断socket正在运行
    if(tcpsocket->isOpen() && tcpsocket->isValid())
    {
        //给服务端发送信息
        tcpsocket->write(str.toUtf8());
        ui->lineEditInput->clear();
    }
    //显示自己发送的信息
    QString showStr = QDateTime::currentDateTime().toString("dddd yyyy.MM.dd hh:mm:ss") + '\n' + str;
    ui->textShowEdit->append(showStr);
}
