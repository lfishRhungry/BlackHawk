#include "tcpsocket.h"

TcpSocket::TcpSocket(QTcpSocket *sock, QObject *parent) : QObject(parent), mSock(sock)
{
    mSock->setParent(this);
    // 将qtcpsocket对应的信号与本对象信号对接起来
    // 如果对接的qtcpsocket发生了相应的信号 那么本对象也要发出相应的信号
    connect(mSock, SIGNAL(readyRead()), this, SLOT(readReady()));
    connect(mSock, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(deleteLater()));

    // 输出信息
    qDebug() << mSock->peerAddress().toString() << ":" << mSock->peerPort() << " 已连接上服务端";
}


void TcpSocket::close()
{
    mSock->disconnectFromHost();
    mSock->close();
}

void TcpSocket::write(QByteArray data)
{
    //
    // 这里可以对写入数据进行加密
    //



    mSock->write(data);

    if (!mSock->waitForBytesWritten(3000)) {
        // 发送数据超时
        close();
        emit disconnected();

        // 输出信息
        qDebug() << mSock->peerAddress().toString() << ":" << mSock->peerPort() \
                 << " 写入失败：" << mSock->errorString();
    }
}

void TcpSocket::readReady()
{
    mBuf.append(mSock->readAll());
    emit newData();
}
