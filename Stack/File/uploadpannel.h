#ifndef UPLOADPANNEL_H
#define UPLOADPANNEL_H

#include <QDialog>
#include <QListWidget>
#include <QScrollArea>
#include <QMap>
#include <QLabel>
#include <QTcpSocket>
#include <Data/UploadFileUnit.h>

namespace Ui {
class UploadPannel;
}

class UploadPannel : public QDialog
{
    Q_OBJECT

public:
    explicit UploadPannel(QWidget *parent = nullptr);
    ~UploadPannel();
    void add_queue(UP_FILE*);
    void touch_upload();
private:
    Ui::UploadPannel *ui;

    QLabel* title;
    QWidget* topbar;
    QListWidget* upload_file_list;

    int uploaded_count = 0;
    QList<QString> upload_queue;    //文件md5的索引
    QMap<QString, UP_FILE*> uploads;   //md5和文件的映射

    bool uploading = false;

    QTcpSocket* s;
};

#endif // UPLOADPANNEL_H









