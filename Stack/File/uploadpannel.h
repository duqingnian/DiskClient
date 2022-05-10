#ifndef UPLOADPANNEL_H
#define UPLOADPANNEL_H

#include <QDialog>
#include <QListWidget>
#include <QScrollArea>
#include <QMap>
#include <QLabel>

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
    void add_upload(UP_FILE*);

    //开始上传
    void start_upload();

    //上传文件到服务器
    void upload_file_to_server(QString filename);

    //更新上传进度
    void sync_upload_state(QString md5, qint64 left_size);

private:
    Ui::UploadPannel *ui;

    QLabel* title;
    QWidget* topbar;
    QListWidget* upload_file_list;

    int uploaded_count = 0;
    bool uploading = false; //是否上传中
    QList<QString> upload_index;    //文件md5的索引
    QMap<QString, UP_FILE*> uploads;   //md5和文件的映射
};

#endif // UPLOADPANNEL_H









