#ifndef DROPDOWNUPLOAD_H
#define DROPDOWNUPLOAD_H

#include <QDialog>

#include <Data/Filter.h>

namespace Ui {
class DropDownUpload;
}

class DropDownUpload : public QDialog
{
    Q_OBJECT

public:
    explicit DropDownUpload(QWidget *parent = nullptr);
    ~DropDownUpload();

    void init();
private:
    Ui::DropDownUpload *ui;
    QVector<FILE_FILTER*> filters;
signals:
    void IntentOpenCreateDialog(QString);
    void IntentUpload(QString);
};

#endif // DROPDOWNUPLOAD_H
