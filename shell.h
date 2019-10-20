// 执行和处理远程shell模块的相关操作

#ifndef SHELL_H
#define SHELL_H

#include <QWidget>
#include "tcpsocket.h"
#include "tcpserver.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QTextEdit>
#include <QTextCodec>
#include <QLineEdit>
#include <QPushButton>

class Shell : public QWidget
{
    Q_OBJECT
public:
    explicit Shell(QWidget *parent = nullptr);
    // 开始远程shell服务器，然后返回新的端口号
    int startShellServer(QString userName);

private:
    TcpServer *mServer;
    TcpSocket *mSock;
    // 界面设计
    QTextEdit *mEditResults;    // 用来显示接收执行结果
    QLineEdit *mEditInput;      // 用来接收用户输入
    QPushButton *mBtSend;       // 用来发送命令
    QPushButton *mBtClear;      // 清空回显和当前命令显示

    // 用于解码GBK
    QTextCodec *codec = QTextCodec::codecForName("GBK");

signals:

public slots:
    // 有新客户连接
    void newConnection(QTcpSocket *s);

    // 处理新数据
    void processBuffer();

protected:
    // 关闭
    void closeEvent(QCloseEvent *);
};

#endif // SHELL_H
