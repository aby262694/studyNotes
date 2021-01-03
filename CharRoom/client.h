#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>

#include <QTcpSocket>

#define SOCKET_COUNT

QT_BEGIN_NAMESPACE
namespace Ui { class client; }
QT_END_NAMESPACE

class client : public QWidget
{
    Q_OBJECT

public:
    client(QWidget *parent = nullptr);
    ~client();

private slots:
    void on_buttonStart_clicked();
    //接收信息
    void readMessage();
    //断开连接槽函数
    void disconnectSlot();

    void on_buttonSend_clicked();

private:
    Ui::client *ui;

    QTcpSocket* tcpsocket;

    //客户端连接状态
    bool connectState;
};
#endif // CLIENT_H









