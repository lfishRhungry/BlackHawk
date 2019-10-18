#include "shell.h"

Shell::Shell(QWidget *parent) : QWidget(parent)
{
    // 初始化窗口
    const int w = 700, h = 400;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - h) >> 1;
    this->setGeometry(x, y, w, h);
    setFixedSize(this->width(),this->height());

    // 输入框
    mEditInput = new QLineEdit(this);
    mEditInput->setGeometry(0, 0, w - 70, 25);
    // 发送按钮
    mBtSend = new QPushButton(this);
    mBtSend->setGeometry(w - 65, 0, 60, 25);
    mBtSend->setText("发送");
    // 当前命令框
    mEditCurrent = new QLineEdit(this);
    mEditCurrent->setGeometry(0, 30, w - 70, 25);
    mEditCurrent->setReadOnly(true);
    // 清空按钮
    mBtClear = new QPushButton(this);
    mBtClear->setGeometry(w - 65, 30, 60, 25);
    mBtClear->setText("清空");
    // 命令回显框
    mEditResults = new QTextEdit(this);
    mEditResults->setGeometry(0, 60, w, 540);
    mEditResults->setReadOnly(true);

    //--------------------------------------按键逻辑-------------------------------------------

    // 清空
    connect(mBtClear, &QPushButton::clicked, [=](){
        mEditCurrent->setText("");
        mEditResults->setText("");
    });

    // 发送
    connect(mBtSend, &QPushButton::clicked, [=](){
        // 注意发送的时候加上windows的换行符 强制换行
        mSock->write(codec->fromUnicode(mEditInput->text() + "\r\n"));
        mEditCurrent->setText(mEditInput->text());
        mEditInput->setText("");
    });

    // 收到新消息
    connect(mSock, &TcpSocket::newData, this, &Shell::processBuffer);


    //--------------------------------------按键逻辑(完)-------------------------------------------

}

int Shell::startShellServer(QString userName){
    // 设置窗口标题
    this->setWindowTitle(userName.append("-远程Shell"));

    // 开启新的服务端
    mServer = new TcpServer(this);
    connect(mServer,SIGNAL(newConnection(QTcpSocket*)), this,SLOT(newConnection(QTcpSocket*)));

    mServer->start(0);
    if (!mServer->server()->isListening()) {
        qDebug() << "开启远程Shell服务端失败";
        deleteLater();
        return -1;
    }

    // 开启半模态监控窗口
    // 阻止对应用的其他窗口进行操作
    // 但是不会阻塞线程
    this->setWindowModality(Qt::ApplicationModal);
    this->show();

    return mServer->server()->serverPort();
}

void Shell::newConnection(QTcpSocket *s)
{
    // 新增客户
    mSock = new TcpSocket(s, this);
    connect(mSock,SIGNAL(newData()), this, SLOT(processBuffer()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(deleteLater()));

    // 不再监听新客户
    mServer->server()->close();
}

void Shell::processBuffer()
{
    // 将数据打印到文本框
    QString text = mEditResults->toPlainText();
    // 针对GBK的解码
    mEditResults->setText(text.append(codec->toUnicode(*mSock->buffer())));

    // 清空缓冲区
    mSock->buffer()->clear();
}

void Shell::closeEvent(QCloseEvent *)
{
    // 删除窗口
    deleteLater();
}
