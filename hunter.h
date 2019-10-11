#ifndef HUNTER_H
#define HUNTER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Hunter; }
QT_END_NAMESPACE

class Hunter : public QWidget
{
    Q_OBJECT

public:
    Hunter(QWidget *parent = nullptr);
    ~Hunter();

private:
    Ui::Hunter *ui;
};
#endif // HUNTER_H
