#ifndef STACKHELP_H
#define STACKHELP_H

#include <QDialog>
#include <QResizeEvent>
#include <QSettings>
#include <QUrl>
#include <Common/basecontroller.h>

namespace Ui {
class StackHelp;
}

class StackHelp : public BaseController
{
    Q_OBJECT

public:
    explicit StackHelp(QWidget *parent = nullptr);
    ~StackHelp();
private:
    Ui::StackHelp *ui;
};

#endif // STACKHELP_H
