// tcp服务端接口
// 会新建一个QTcpServer对象
// 通过tcpserver类新建监听的服务器对象
// 可以一直处于监听状态 并接受连接返回对于qtcpsocket对象


#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    // 启动服务端
    void start(int port);
    void stop();

    // 反回服务器
    QTcpServer *server() {
        return mServer;
    }

private:
    QTcpServer *mServer;
signals:
    // 当新的连接进来时发送的信号
    // @sock 一旦有新连接，他就会将新连接的socket用信号发射给调用他的类
    void newConnection(QTcpSocket *sock);

public slots:
    // 当有从mServer中接收到新连接后，获取新连接的socket，然后再
    // 发射newConnection信号
    void newConnection();
};

#endif // TCPSERVER_H
