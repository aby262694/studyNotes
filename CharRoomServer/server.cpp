#include "server.h"
#include "ui_server.h"

#include <QHostAddress>
#include <QNetworkInterface>
#include <QStringList>
#include <QMessageBox>
#include <QDateTime>

#include <QDebug>
#define cout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

server::server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::server)
{
    ui->setupUi(this);

    //初始化
    init();
}

server::~server()
{
    delete ui;

    tcpServer->close();
    delete tcpServer;
}

//初始化
void server::init()
{
    enumAllIp();

    //创建监听socket
    tcpServer = new QTcpServer(this);
    clientSocket = NULL;
    //有新连接时传入信号
    connect(tcpServer, &QTcpServer::newConnection, this, &server::newConnectSlot);

    //列表模型
    listModel = new QStringListModel(this);
    strlist.clear();
}
//初始化本机所有可用的ipv4的地址并添加地址到列表框中
void server::enumAllIp()
{
    //接收本机所有Ip地址
    QList <QHostAddress> addressList = QNetworkInterface::allAddresses();
    QStringList addressList_str;
    for(int i = 0; i < addressList.size(); i++)
    {   //地址为空则跳过
        if(addressList.at(i).isNull()) continue;
        //协议族不是ipv4的则跳过
        if(addressList.at(i).protocol() != QAbstractSocket::IPv4Protocol) continue;
        //符合条件的地址添加到列表中
        addressList_str.append(addressList.at(i).toString());
    }
    //将地址列表添加到地址列表框中
    ui->comboBoxIp->addItems(addressList_str);
}

void server::on_buttonListen_clicked()
{
    QString currentIp = ui->comboBoxIp->currentText();     //当前ip列表的Ip
    quint16 currentPort = ui->lineEditPort->text().toInt();       //当前文本框的显示的端口
    QHostAddress currentHostIP = QHostAddress(currentIp);   //地址字符串转换成主机Ip
    //客户端正在监听
    if( tcpServer->isListening())
    {
        ui->buttonListen->setEnabled(false);
        ui->buttonListen->setText("正在运行");
    }
    else    //客户端没有在监听
    {
        if(tcpServer->listen(currentHostIP, currentPort))   //监听成功返回true
        {
            ui->buttonListen->setEnabled(false);
            ui->buttonListen->setText("正在运行");
        }
        else    //监听失败
        {
            cout << "listen error";
            //出现错误提示消息框
            QMessageBox::warning(this, "listen error", tcpServer->errorString());
        }
    }
}

void server::newConnectSlot()
{
    //获取当前连接的客户端套接字
    clientSocket = tcpServer->nextPendingConnection();
    //将连接的客户端放入客户端列表中
    clientList_socket.push_back(clientSocket);

    QString str = clientSocket->peerAddress().toString() + ": " + QString::number(clientSocket->peerPort()) + "已经连接";

    strlist.append(str);
    listModel->setStringList(strlist);  //将连接信息放在列表窗口中
    ui->listViewConnectShow->setModel(listModel);
    //准备接受信息
    connect(clientSocket, &QTcpSocket::readyRead, this, &server::readMessage);
    //客户端的断开连接信息
    connect(clientSocket, &QTcpSocket::disconnected, this, &server::disConnectSlot);
}

//读取客户端信息及给其他客户端发送信息
void server::readMessage()
{
    QTcpSocket*  currentClient;
    QByteArray arr;
    QString str;
    //有客户端存在
    if(!clientList_socket.isEmpty())
    {   //服务端接收信息
        for(int i = 0; i < clientList_socket.count(); i++)
        {   //接收客户端发送的字节信息
            arr = clientList_socket.at(i)->readAll();
            if(arr.isNull())
            {   //空代表不是该客户端发送
                continue;
            }
            currentClient = clientList_socket.at(i);
            str = QDateTime::currentDateTime().toString("dddd.yyyy.MM.dd HH:mm:ss") + '\n' + arr.data();
            break;
        }
        //显示信息
        ui->messageShow->append(str);
        //给其它客户端发送信息
        for(int i = 0; i < clientList_socket.count(); i++)
        {
            QTcpSocket *temp = clientList_socket.at(i);
            //遇到自己就跳过
            if(currentClient == temp)
            {
                continue;
            }
            temp->write(str.toUtf8());   //发送信息
        }
    }
}
//客户端断开连接时服务端显示断开信息
void server::disConnectSlot()
{
    QString closeStr = QDateTime::currentDateTime().toString("dddd.yyyy.MM.dd HH:mm:ss") + ' ' + "clint close";
    strlist.append(closeStr);
}

//给客户端发送消息
void server::on_buttonSend_clicked()
{
    QString sendStr = ui->lineEditSend->text();
    for(int i = 0; i < clientList_socket.count(); i++)
    {
        clientList_socket.at(i)->write(sendStr.toUtf8());
        ui->lineEditSend->clear();
    }
    QString showStr = QDateTime::currentDateTime().toString("dddd.yyyy.MM.dd HH:mm:ss") + '\n' + sendStr;
    ui->messageShow->append(showStr);    //在接收信息框显示自己发送的信息
}
