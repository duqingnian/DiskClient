#pragma execution_character_set("utf-8")
#include "uploadfilelisteitem.h"
#include "ui_uploadfilelisteitem.h"
#include <QDebug>
#include <QLabel>

UploadFileListeItem::UploadFileListeItem(QWidget *parent) :QDialog(parent),ui(new Ui::UploadFileListeItem)
{
    ui->setupUi(this);
    ui->progressBar->setStyleSheet("QProgressBar{border:0px;background-color:#FFFFFF;}QProgressBar::chunk{background-color:#eaf9df;}");
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
    ui->file_state->setText("状态");

    ui->file_icon->setPixmap(QPixmap::fromImage(QImage(":/Resources/types/"+upload_file->type+".png")));
    ui->file_icon->resize(32,32);
    ui->file_icon->setScaledContents(true);
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
