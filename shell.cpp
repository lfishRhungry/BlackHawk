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
    mEditCurrent->setGeometry(0, 30, w, 25);
    mEditCurrent->setReadOnly(true);
    // 命令回显框
    mEditResults = new QTextEdit(this);
    mEditResults->setGeometry(0, 60, w, 540);
    mEditResults->setReadOnly(true);

    //--------------------------------------按键逻辑-------------------------------------------
    mBtSend->setEnabled(false);
    connect(mEditInput, &QLineEdit::textChanged, [=](const QString& text){
        if (!text.isEmpty()){
            mBtSend->setEnabled(true);
        }else {
            mBtSend->setEnabled(false);
        }
    });

    //--------------------------------------按键逻辑(完)-------------------------------------------

    // 测试
    this->show();
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
