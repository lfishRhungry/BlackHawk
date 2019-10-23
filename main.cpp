#include "hunter.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 设置字体
    QFont f("Monaco");
    a.setFont(f);

    Hunter w;
    w.show();
    return a.exec();
}
