#include "proc.h"

Proc::Proc(QWidget *parent) : QWidget(parent), mSock(nullptr)
{
    // 初始化窗口
    const int w = 600, h = 500;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - h) >> 1;
    this->setGeometry(x, y, w, h);

    // 进程列表
    mtableProc = new QTableWidget(this);
    mtableProc->setGeometry(0, 0, w, h);
    mtableProc->setColumnCount(3);
    mtableProc->setHorizontalHeaderItem(0, new QTableWidgetItem("PID"));
    mtableProc->setHorizontalHeaderItem(1, new QTableWidgetItem("进程名"));
    mtableProc->setHorizontalHeaderItem(2, new QTableWidgetItem("所有者"));
    // 自适应列宽
    mtableProc->horizontalHeader()->setStretchLastSection(true);
    mtableProc->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    // 隐藏水平表头
//    mtableProc->verticalHeader()->setHidden(true);
    // 一行一行地选中
    mtableProc->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 一次最多选中一行
    mtableProc->setSelectionMode(QAbstractItemView::SingleSelection);
    // 设置不能修改
    mtableProc->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 对进程列表的右键菜单
    mProcMenu = new QMenu(this);
    QAction *actProcRefresh = mProcMenu->addAction("刷新进程列表");
    connect(actProcRefresh,SIGNAL(triggered(bool)), this, SLOT(refreshProcTbl()));
    QAction *actKillProc = mProcMenu->addAction("Kill(建议先刷新)");
    connect(actKillProc,SIGNAL(triggered(bool)), this,SLOT(killProc()));

    // 在列表中增加鼠标事件监控，当右击点下时就能弹出菜单
    mtableProc->installEventFilter(this);
}

int Proc::startProcServer(QString userName){
    // 设置窗口标题
    _userName = userName;
    this->setWindowTitle(userName.append("-进程管理"));

    // 开启新的服务端
    mServer = new TcpServer(this);
    connect(mServer,SIGNAL(newConnection(QTcpSocket*)), this,SLOT(newConnection(QTcpSocket*)));

    mServer->start(0);
    if (!mServer->server()->isListening()) {
        qDebug() << "开启文件监控服务端失败";
        deleteLater();
        return -1;
    }

    // 开启监控窗口
    this->show();

    return mServer->server()->serverPort();
}

void Proc::addProcToTbl(int pid, QString exeName, QString owner){
    // 增加一行
    int count = mtableProc->rowCount();
    mtableProc->setRowCount(count + 1);

    // 添加信息
    QTableWidgetItem *itemPid = new QTableWidgetItem(QString::number(pid));
    mtableProc->setItem(count, 0 , itemPid);

    QTableWidgetItem *itemExeName = new QTableWidgetItem(exeName);
    mtableProc->setItem(count, 1 , itemExeName);

    QTableWidgetItem *itemOwner = new QTableWidgetItem(owner);
    mtableProc->setItem(count, 2 , itemOwner);

}

int Proc::curProcPidInTbl(){
    return mtableProc->item(mtableProc->currentRow(), 0)->text().toInt();
}

void Proc::processCommand(QByteArray &cmd, QByteArray &args)
{
    cmd = cmd.toUpper().trimmed();
    QHash<QByteArray, QByteArray> hashArgs = parseArgs(args);

    // 新的进程信息
    if (cmd == CmdSendProc) {
        doAddProc(hashArgs);
        return;
    }

    // kill进程成功
    if (cmd == CmdKillProcSuccess) {
        QMessageBox::information(this, "提示","主人棒棒哒，kill进程成功，将刷新列表！");
        refreshProcTbl();
        return;
    }

    // kill进程失败
    if (cmd == CmdKillProcSuccess) {
        QMessageBox::warning(this, "提示","对不起主人，kill进程失败");
        return;
    }
}

void Proc::doAddProc(QHash<QByteArray, QByteArray> &args){
    // 这里解一下码 以防万一
    addProcToTbl(args["PID"].toInt(), codec->toUnicode(args["EXENAME"]),
            codec->toUnicode(args["OWNER"]));
}

QHash<QByteArray, QByteArray> Proc::parseArgs(QByteArray &args)
{
    QList<QByteArray> listArgs = args.split(CmdSplit[0]);

    // 分解参数，然后把它加入哈希表
    QHash<QByteArray, QByteArray> hashArgs;
    for(int i=0; i<listArgs.length()-1 ; i+=2) {
        hashArgs.insert(listArgs[i].toUpper().trimmed(),
                        listArgs[i+1].trimmed());
    }

    return hashArgs;
}

void Proc::killProc(){
    if (mSock){
        QString data;
        data.append(CmdKillProc + CmdSplit);
        data.append("PID" + CmdSplit);
        data.append(QString::number(curProcPidInTbl(), 10) + CmdEnd);
        mSock->write(codec->fromUnicode(data));
    }

}

void Proc::refreshProcTbl(){
    if(mSock){
        // 清空当前进程列表
//        int count = mtableProc->rowCount();
//        for (int i = 0; i< count; i++) {
//            mtableProc->removeRow(i);
//        }
        mtableProc->setRowCount(0);

        // 发送刷新消息
        QString data;
        data.append(CmdFreshProcs +CmdEnd);
        mSock->write(codec->fromUnicode(data));
    }
}

void Proc::newConnection(QTcpSocket *s)
{
    // 新增食物
    mSock = new TcpSocket(s, this);
    connect(mSock,SIGNAL(newData()), this, SLOT(processBuffer()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(deleteLater()));

    // 刷新进程列表
    refreshProcTbl();

    // 不再监听新客户
    mServer->server()->close();
}

void Proc::processBuffer()
{
    // 从socket里获取缓存区
    QByteArray *buf = mSock->buffer();

    int endIndex;
    while ((endIndex = buf->indexOf(CmdEnd)) > -1) {
        // 提取一行指令
        QByteArray data = buf->mid(0, endIndex);
        buf->remove(0, endIndex + CmdEnd.length());

        // 提取指令和参数
        QByteArray cmd, args;
        int argIndex = data.indexOf(CmdSplit);
        if (argIndex == -1) {
            cmd = data;
        } else {
            cmd = data.mid(0, argIndex);
            args = data.mid(argIndex+CmdSplit.length(), data.length());
        }

        // 处理指令
        processCommand(cmd, args);
    }
}

bool Proc::eventFilter(QObject *watched, QEvent *event)
{
    // 右键弹出菜单
    if (watched == (QObject*)mtableProc) {
        if (event->type() == QEvent::ContextMenu) {
            mProcMenu->exec(QCursor::pos());
        }
    }

    return QObject::eventFilter(watched, event);
}

void Proc::resizeEvent(QResizeEvent *)
{
    mtableProc->setGeometry(0,0,width(), height());
}

void Proc::closeEvent(QCloseEvent *)
{
    // 删除窗口
    deleteLater();
}
