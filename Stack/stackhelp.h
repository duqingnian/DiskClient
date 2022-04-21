#ifndef STACKHELP_H
#define STACKHELP_H

#include <QDialog>

namespace Ui {
class StackHelp;
}

class StackHelp : public QDialog
{
    Q_OBJECT

public:
    explicit StackHelp(QWidget *parent = nullptr);
    ~StackHelp();

private:
    Ui::StackHelp *ui;
};

#endif // STACKHELP_H
