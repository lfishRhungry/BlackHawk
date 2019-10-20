// 辅助文件模块进行文件传输的类

#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QObject>

class FileTransfer : public QObject
{
    Q_OBJECT
public:
    explicit FileTransfer(QObject *parent = nullptr);

signals:

public slots:
};

#endif // FILETRANSFER_H
