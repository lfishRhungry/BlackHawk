// ddos模块用来获取目标ip和端口的弹窗

#ifndef DDOS_H
#define DDOS_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QLineEdit>
#include <QValidator>
#include <QPushButton>
#include <QDialog>

class Ddos : public QDialog
{
    Q_OBJECT
public:
    explicit Ddos(QDialog *parent = nullptr);
    int getPORT();
    QString getIP();

private:
    QLineEdit *mEditIP;
    QLineEdit *mEditPORT;
};

#endif
