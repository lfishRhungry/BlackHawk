// tcpsocket是对已有的QTcpSocket和功能需求的封装
// 通过使用tcpsocket的封装的函数来对相应的qtcpsocket进行数据发送和接受
// 这里主要封装的内容就是将qtcpsocket与对应存储读出数据的缓冲区绑定在了一起
// 一旦从客户里接收到数据，他就会将接收到数据的信号发射给调用他的类


#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>


class TcpSocket : public QObject
{
    Q_OBJECT
public:
    // 初始化socket
    // @sock: 把sock加到这个类mSock的私有变量中
    explicit TcpSocket(QTcpSocket *sock, QObject *parent = nullptr);

    // 获取socket
    QTcpSocket *socket() {
        return mSock;
    }

    // 获取缓存区
    QByteArray *buffer() {
        return &mBuf;
    }

    // 断开和食物之间的连接
    void close();

    // 发送数据
    void write(QByteArray data);

private:
    QTcpSocket *mSock;  // 食物
    QByteArray mBuf;    // 数据缓冲区，从食物里接收到的数据都会先放在这里

signals:
    // 这里的newdata和disconnected本来是qtcpsocket的信号，这里由于对
    // qtcpsocket进行功能封装 也对其设计了相应的这几个信号和对应的逻辑

    // 当有新数据加入到mBuf后就发射这个信号，让调用这个类的类知道，
    // 然后在对新的数据作出相应的处理
    void newData();

    // 当客户断开时发射的信号
    void disconnected();

public slots:
    // 将msock内容读到缓冲区
    void readReady();
};

#endif // TCPSOCKET_H
