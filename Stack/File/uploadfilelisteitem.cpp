#pragma execution_character_set("utf-8")
#include "uploadfilelisteitem.h"
#include "ui_uploadfilelisteitem.h"
#include <QDebug>
#include <QLabel>

UploadFileListeItem::UploadFileListeItem(QWidget *parent) :QDialog(parent),ui(new Ui::UploadFileListeItem)
{
    ui->setupUi(this);
    this->setStyleSheet("font-size:12px;");
    //ui->progressBar->setStyleSheet("QProgressBar{border:0px;background-color:#FFFFFF;}QProgressBar::chunk{background-color:#eaf9df;}");
    ui->progressBar->setStyleSheet("QProgressBar{border:0px;background-color:#FFFFFF;}");
    //ui->file_speed->setStyleSheet("#file_speed{font-size:10px;}");
}

UploadFileListeItem::~UploadFileListeItem()
{
    delete ui;
}

void UploadFileListeItem::set_upload_file(UP_FILE* up_file)
{
    upload_file = up_file;
    ui->file_name->setText(upload_file->name);
    ui->file_size->setText(ConverSize(upload_file->size));
    ui->file_state->setText("准备上传");

    ui->file_icon->setPixmap(QPixmap::fromImage(QImage(":/Resources/types/"+upload_file->ico+".png")));
    ui->file_icon->resize(26,26);
    ui->file_icon->setScaledContents(true);

    ui->progressBar->setMaximum(100);
}

QString UploadFileListeItem::ConverSize(unsigned long long bytes)
{
    float size = bytes;
    QList<QString> suffix = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    int total = suffix.count();
    int i = 0;
    for (; size > 1024 && i < total; i++)
    {
        size /= 1024;
    }
    return QString::number(size,'f',2) +" "+ suffix[i];
}

void UploadFileListeItem::set_progress(float val)
{

    if(val >= 100)
    {
        val = 100;
        ui->file_state->setText("同步中");
    }
    else
    {
        ui->file_state->setText(QString::number(val,'f',2)+"%");
    }
    ui->progressBar->setValue(val);
}

void UploadFileListeItem::set_speed(unsigned long long speed)
{
    if(speed > 0)
    {
        ui->file_speed->setText(ConverSize(speed)+"/秒");
    }
}

void UploadFileListeItem::completed()
{
    ui->file_state->setPixmap(QPixmap::fromImage(QImage(":/Resources/Common/complete.png")));
    ui->file_state->resize(26,26);
    ui->file_state->setScaledContents(true);
}
