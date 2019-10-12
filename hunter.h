#ifndef HUNTER_H
#define HUNTER_H

#include <QWidget>
#include <QDebug>
#include <QIcon>
#include <QPushButton>
#include <QSizePolicy>
#include <QMenu>
#include <QAction>

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
    QMenu *m_Rclick; // 右键菜单

public slots:

    // 添加食物列表
    void addFoodToTbl(int id, QString username, QString ipaddr, int port, QString sysInfo);
    // 删除食物
    void rmFoodFromTbl(int id);
    // 返回当前选中食物ID
    int curFoodIdInTbl();

    // 开启服务器监听
    void startLstn();

protected:
    // 重写事件过滤函数
    bool eventFilter(QObject *watched, QEvent *event);
};
#endif // HUNTER_H
