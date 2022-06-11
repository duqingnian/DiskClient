#pragma execution_character_set("utf-8")
#include "filerow.h"
#include "ui_filerow.h"
#include <QDebug>

FileRow::FileRow(QWidget *parent) : QDialog(parent),ui(new Ui::FileRow)
{
    ui->setupUi(this);

    this->setCursor(Qt::PointingHandCursor);
}

FileRow::~FileRow()
{
    delete ui;
}

void FileRow::set_file(FD* _file)
{
    file = _file;

    ui->label_name->setText(file->name);
    ui->label_version->setText(file->version);
    ui->label_user->setText(file->creator);
    if("FOLDER" == file->suffix)
    {
        ui->label_size->setText("");
    }
    else
    {
        ui->label_size->setText(ConverSize(file->size));
    }

    ui->label_ico->setPixmap(QPixmap::fromImage(QImage(file->icon)));
    ui->label_ico->resize(30,30);
    ui->label_ico->setScaledContents(true);
}

QString FileRow::ConverSize(unsigned long long bytes)
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

void FileRow::resize(int width)
{
    if(width > 1400)
    {
        ui->label_name->resize(865,50);
    }
    else
    {
        ui->label_name->resize(475,50);
    }
    ui->label_user->move(ui->label_name->x() + ui->label_name->width() + 10,0);
    ui->label_version->move(ui->label_user->x() + ui->label_user->width(),0);
    ui->label_size->move(ui->label_version->x() + ui->label_version->width(),0);
}

