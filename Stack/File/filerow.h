#ifndef FILEROW_H
#define FILEROW_H

#include <QDialog>
#include <Data/FD.h>

namespace Ui {
class FileRow;
}

class FileRow : public QDialog
{
    Q_OBJECT

public:
    explicit FileRow(QWidget *parent = nullptr);
    ~FileRow();
    FD* file;
    //设置文件对象
    void set_file(FD*);
    //字节转KB，MB，GB
    QString ConverSize(unsigned long long);

    void resize(int width);
private:
    Ui::FileRow *ui;
};

#endif // FILEROW_H
