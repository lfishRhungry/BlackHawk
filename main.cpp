#include "hunter.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Hunter w;
    w.show();
    return a.exec();
}
