﻿#ifndef UPLOADPANNEL_H
#define UPLOADPANNEL_H

#include <QDialog>
#include <Common/basecontroller.h>
#include <QListWidget>
#include <QScrollArea>
#include <QMap>
#include <QLabel>
#include <QTcpSocket>
#include <Data/UploadFileUnit.h>
#include <Component/Label.h>
#include <QElapsedTimer>
#include <QSettings>
#include <QSize>
#include <QThread>
#include <QTimer>
#include "Common/filemanager.h"
#include <Component/Msg.h>
#include <Component/Toast.h>
#include <Lib/HttpClient.h>

namespace Ui {
class UploadPannel;
}

class UploadPannel : public BaseController
{
    Q_OBJECT

public:
    explicit UploadPannel(QWidget *parent = nullptr);
    ~UploadPannel();
    void add_queue(UP_FILE*);
    void touch_upload(QString,int,int,QString);
    void clear_queue();//清除上传面板
    qintptr  GetSocketDescriptor(); //获取socket
private:
    Ui::UploadPannel *ui;
    qintptr file_socket_descriptor;

    QString SJN = "";

    QLabel* title;
    Label* topbar;
    QListWidget* upload_file_list;

    int uploaded_count = 0;
    QList<QString> upload_queue;    //文件md5的索引
    QMap<QString, UP_FILE*> uploads;   //md5和文件的映射
    QList<QString> complete_queue;    //已经上传完毕的集合

    bool uploading = false;

    Label* clear;
    Label* min;
    Label* max;
    Label* close;

    QString show_flag="";

    QTcpSocket* send_socket;
public slots:
    void _clear_upoload_queue(); //清空上传列表
    void clear_upoload_queue();
    void close_upload_pannel(); //关闭上传面板

    void onMin();
    void onMax();

    void readyRead();
    void disconnected();

    //更新上传文件的进度
    void sync_file_progrrss(QString BUNDLE,QString BUNDLE_ID,QString FD_ID,QString md5,QString state,float pct,unsigned long long SPEED,unsigned long long send_size);
signals:
    void do_some_action(QString);

};

#endif // UPLOADPANNEL_H









