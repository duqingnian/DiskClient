#ifndef UPLOADFILELISTEITEM_H
#define UPLOADFILELISTEITEM_H

#include <QDialog>
#include <Data/UploadFileUnit.h>

namespace Ui {
class UploadFileListeItem;
}

class UploadFileListeItem : public QDialog
{
    Q_OBJECT

public:
    explicit UploadFileListeItem(QWidget *parent = nullptr);
    ~UploadFileListeItem();

    UP_FILE* upload_file;

    //设置文件对象
    void set_upload_file(UP_FILE*);

    //字节转KB，MB，GB
    QString ConverSize(unsigned long long);

    //设置进度条
    void set_progress(float);

    //设置速度
    void set_speed(unsigned long long);

    //设置为完成
    void completed();

private:
    Ui::UploadFileListeItem *ui;
};

#endif // UPLOADFILELISTEITEM_H
