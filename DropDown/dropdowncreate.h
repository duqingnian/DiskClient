#ifndef DROPDOWNCREATE_H
#define DROPDOWNCREATE_H

#include <QDialog>

#include <Data/Filter.h>

namespace Ui {
class DropDownCreate;
}

class DropDownCreate : public QDialog
{
    Q_OBJECT

public:
    explicit DropDownCreate(QWidget *parent = nullptr);
    ~DropDownCreate();

    void init();

private:
    Ui::DropDownCreate *ui;
    QVector<FILE_FILTER*> filters;
signals:
    void IntentOpenCreateDialog(QString);
};

#endif // DROPDOWNCREATE_H
