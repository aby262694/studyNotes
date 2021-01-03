#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QStringListModel>
#include <QStringList>

QT_BEGIN_NAMESPACE
namespace Ui { class server; }
QT_END_NAMESPACE

class server : public QWidget
{
    Q_OBJECT

public:
    server(QWidget *parent = nullptr);
    ~server();
    //初始化
    void init();
    //存储本机所有可用的ipv4地址
    void enumAllIp();


private slots:
    //新连接客户端槽函数
    void newConnectSlot();
    //客户端断开连接后处理
    void disConnectSlot();
    //接收信息的槽函数
    void readMessage();

    void on_buttonListen_clicked();

    void on_buttonSend_clicked();

private:
    Ui::server *ui;

    QTcpServer *tcpServer;   //服务端
    QList <QTcpSocket*> clientList_socket;   //多个客户端的套接字列表
    QTcpSocket *clientSocket;

    QStringListModel *listModel;
    QStringList strlist;
};
#endif // SERVER_H
