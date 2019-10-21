// 辅助文件模块进行文件传输的类

#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QObject>
#include <QProgressDialog>
#include "tcpserver.h"
#include "tcpsocket.h"
#include <QFile>
#include <QDir>

class FileTransfer : public QObject
{
    Q_OBJECT
public:
    explicit FileTransfer(QObject *parent = nullptr);

    // 文件数据包头
    typedef struct {
        char fileName[256];
        unsigned int len;
    } FileHeader;

    // 开始文件接收服务器，然后返回新的端口号
    int startRecvFileServer(QString userName, QString filePath);

    // 开始文件传送服务器，然后返回新的端口号
    int startSendFileServer(QString userName, QString filePath);

private:
    QProgressDialog *mProgress; // 进度条窗口
    TcpServer *mServer;     // hunter端
    TcpSocket *mSock;     // 食物端
    QFile mFile;        // 文件
    FileHeader mHeader; // 文件头
    unsigned int _curWritten;       // 已经写入数据

signals:

public slots:
    void close();

    // 食物连接
    void newRecvFileConnection(QTcpSocket *);
    void newSendFileConnection(QTcpSocket *);

    // 接收文件数据
    void recvData();

    // 发送数据
    void sendData();
};

#endif // FILETRANSFER_H
