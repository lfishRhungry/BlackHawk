#include "ddos.h"

Ddos::Ddos(QDialog *parent) : QDialog(parent)
{
    const int w = 300, h = 120;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - h) >> 1;
    this->setGeometry(x, y, w, h);

    QLabel *IP=new QLabel("目标 IP",this);
    IP->setGeometry(30,20,60,30);
    mEditIP=new QLineEdit(this);
    mEditIP->setText("0.0.0.0");
    mEditIP->setMaxLength(80);
    mEditIP->setGeometry(105,20,80,30);

    QLabel *PORT=new QLabel("目标PORT",this);
    PORT->setGeometry(30,70,60,30);
    mEditPORT=new QLineEdit(this);
    mEditPORT->setText("80");
    mEditPORT->setValidator(new QIntValidator(1,65535));
    mEditPORT->setGeometry(105,70,80,30);

    QPushButton *ATK=new QPushButton("攻击",this);
    connect(ATK,SIGNAL(clicked(bool)),this,SLOT(close()));
    ATK->setGeometry(200,20,80,80);
    // 强制模式对话框
    this->setAttribute(Qt::WA_ShowModal,true);
    this->exec();
}


int Ddos::getPORT()
{
    return mEditPORT->text().toInt();
}

QString Ddos::getIP()
{
    return mEditIP->text();
}
