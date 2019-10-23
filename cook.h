// 主要的服务端
// 对tcpserver的一个高级封装
// 管理多个食物

#ifndef COOK_H
#define COOK_H

#include <QObject>
#include "tcpserver.h"
#include "food.h"
#include <QHash>

class Cook : public QObject
{
    Q_OBJECT
public:
    explicit Cook(QObject *parent = nullptr);
    // 启动或停止服务端
    void start(int port);
    void stop();

    // 用id来获取food
    //调用了food中的函数
    Food *getFoodById(int id) {
        return mFoods[id];
    }

    TcpServer *GetTcpServer() {
        return mServer;
    }

private:
    TcpServer *mServer;         // Tcp服务端
    QHash<int, Food*> mFoods;  // 用ID来索引相应的客户

    // 生成新的id
    int generateId();

signals:
    // 客户登入或登出，主要是告诉窗口控件
    void foodLogin(int id, QString userName,
                     QString ip,int port, QString system, QString processor);
    void foodLogout(int id);

public slots:
    // 新客户连接
    void newConnection(QTcpSocket *sock);

    // 客户登入
    void login(Food*, QString userName,
                  QString ip, int port, QString system, QString processor);

    // 客户登出
    void logout(int id);
};

#endif // COOK_H
