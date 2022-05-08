#pragma execution_character_set("utf-8")
#include "uploadpannel.h"
#include "ui_uploadpannel.h"
#include "uploadfilelisteitem.h"
#include <QAbstractItemView>
#include <QDebug>
#include <QSize>

UploadPannel::UploadPannel(QWidget *parent):QDialog(parent),ui(new Ui::UploadPannel)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    topbar = new QWidget(this);
    topbar->setObjectName("topbar");
    topbar->resize(495,38);
    topbar->setStyleSheet("border: 1px solid #0d64b0;background-color: #1361b0;border-top-left-radius: 5px;border-top-right-radius: 5px;");
    topbar->move(0,0);

    title = new QLabel(topbar);
    title->setObjectName("title");
    title->setText("上传 (0/0)");
    title->setStyleSheet("#title{color:#fff;font-size:16px;}");
    title->move(13,7);
    title->setMinimumWidth(175);

    QLabel* close = new QLabel(topbar);
    close->setStyleSheet("background:transparent;");
    QImage closeImg(":/Resources/Common/close_w.png");
    close->setPixmap(QPixmap::fromImage(closeImg));
    close->resize(20,20);
    close->move(460,9);

    QString css = "#upload_file_list{border:0px;background:#fff;}";
    upload_file_list = new QListWidget(this);
    upload_file_list->setObjectName("upload_file_list");
    upload_file_list->setStyleSheet(css);
    upload_file_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    upload_file_list->resize(493,56*6);
    upload_file_list->move(1,38);
}

UploadPannel::~UploadPannel()
{
    delete ui;
}

void UploadPannel::add_upload(UP_FILE* upload_file)
{
    if(uploads.count(upload_file->md5) > 0)
    {
        return;
    }
    uploads[upload_file->md5] = upload_file;

    UploadFileListeItem* upload_file_item = new UploadFileListeItem();
    upload_file_item->set_upload_file(upload_file);

    QListWidgetItem* item = new QListWidgetItem(upload_file_list);
    item->setSizeHint(QSize(494,56));
    upload_file_list->setItemWidget(item, upload_file_item);
    title->setText(QString("上传 (0/%1)").arg(upload_file_list->count()));
    upload_file_list->setCurrentRow(upload_file_list->count()-1);
}





