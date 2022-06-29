#pragma execution_character_set("utf-8")
#include "dirprocess.h"
#include "ui_dirprocess.h"
#include <QDebug>
#include <QMovie>
#include <QSettings>
#include <Thread/uploaddirthread.h>

DirProcess::DirProcess(QWidget *parent) :QDialog(parent),ui(new Ui::DirProcess)
{
    ui->setupUi(this);
    this->setStyleSheet("font-family: \"Microsoft Yahei\";");

    this->setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    ui->label_icon->setStyleSheet("background:transparent;");
    ui->label_icon->setPixmap(QPixmap::fromImage(QImage(":/Resources/Common/folder.png")));

    //    connect(ui->btn_cancel,&QPushButton::clicked,this,[=](){
    //        emit do_something("CANCEL");
    //    });

    uploaded_file_num = 0;
    ui->label_complete->hide();
    ui->widget_prepareing->hide();
    ui->widget_uploading->hide();
    this->init(0);

    seconds = 0;
    connect(&timer,&QTimer::timeout,this,[=](){
        seconds++;

        int min = 0;
        int sec = 0;

        if(seconds >= 60)
        {
            min = seconds/60;
            sec = seconds % 60;
        }
        else
        {
            sec = seconds;
        }
        ui->btn_time_tip->setText("用时: "+QString::number(min)+" 分 "+QString::number(sec)+" 秒");
    });

    connect(&timer_used,&QTimer::timeout,this,[=](){
        seconds++;

        int hour = 0;
        int min = 0;
        int sec = 0;

        if(seconds >= 60)
        {
            min = seconds/60;
            sec = seconds % 60;
            if(min >= 60)
            {
                hour = min/60;
                min = min%60;
            }
        }
        else
        {
            sec = seconds;
        }
        ui->label__time_used->setText(QString::number(hour)+" 时 "+QString::number(min)+" 分 "+QString::number(sec)+" 秒");
    });
}

void DirProcess::init(int id)
{
    dir_count = 0;
    file_count = 0;
    size = 0;
    uploaded_file_num = 0;

    movie_loading = new QMovie(":/Resources/load.gif");
    ui->label_loading->setMovie(movie_loading);
    ui->label_loading->resize(30,30);
    ui->label_loading->setScaledContents(true);
    movie_loading->start();

    movie_complete = new QMovie(":/Resources/Common/complete.png");
    ui->label_complete->setMovie(movie_complete);
    ui->label_complete->resize(30,30);
    ui->label_complete->setScaledContents(true);
    movie_complete->start();

    seconds = 0;
    ui->label_complete->hide();
    ui->label_loading->show();

    ui->widget_prepareing->hide();
    ui->widget_uploading->hide();
    ui->widget_calculate->show();
    ui->widget_calculate->move(30,45);
    this->resize(500,345);
}

//准备上传中
void DirProcess::changeto_prepareing()
{
    uploaded_file_num = 0;

    seconds = 0;
    timer.stop();

    ui->widget_calculate->hide();
    ui->widget_uploading->hide();

    ui->label__file_num_progress->setText("0 / "+QString::number(file_count));

    ui->widget_prepareing->move(30,50);
    ui->widget_prepareing->show();
    this->resize(500,100);
}

//正在上传
void DirProcess::changeto_uploading(unsigned long long bytes)
{
    total_size = (double)bytes;

    ui->widget_calculate->hide();
    ui->widget_prepareing->hide();

    ui->progressBarCurrFile->setMaximum(1000);
    ui->progressBarCurrFile->setValue(0);

    ui->progressBar_uploading->setMaximum(1000);
    ui->progressBar_uploading->setValue(0);
    uploaded = 0;

    ui->label_disk_ico->setPixmap(QPixmap::fromImage(QImage(":/Resources/dialog/disk.png")));
    ui->label_disk_ico->resize(50,50);
    ui->label_disk_ico->setScaledContents(true);

    ui->widget_uploading->move(30,30);
    ui->widget_uploading->show();
    this->resize(500,260);

    timer_used.start(1000);
}

void DirProcess::update_upload_progress(int num)
{
    uploaded += (double)num;
    if(uploaded >= total_size)
    {
        uploaded = total_size;
    }
    double pct = (uploaded / total_size)*1000;
    ui->progressBar_uploading->setValue(pct);
    ui->label__sended_size->setText("已上传: "+ConverSize(uploaded)+" , 总大小: "+ConverSize(total_size));
}

void DirProcess::clear_cache_data()
{
    uploaded = 0;
    total_size = 0;
    ui->progressBar_uploading->setMaximum(100);
    ui->progressBar_uploading->setValue(0);

    ui->progressBarCurrFile->setMaximum(1000);
    ui->progressBarCurrFile->setValue(0);
}

DirProcess::~DirProcess()
{
    delete ui;
}

void DirProcess::set_data(QString name, QString data)
{
    if("dir_path" == name)
    {
        dir_path = data;
        ui->label_dir_path->setText(data);
        timer.start(1000);
    }
    else if("dir_name" == name)
    {
        ui->label_dir_name->setText(data);
    }
    else if("ADD_FILE" == name)
    {
        file_count++;
        ui->label_file_count->setText(QString::number(dir_count)+" 个文件夹， "+QString::number(file_count)+" 个文件");
    }
    else if("ADD_DIR" == name)
    {
        dir_count++;
        ui->label_file_count->setText(QString::number(dir_count)+" 个文件夹， "+QString::number(file_count)+" 个文件");
    }
    else if("ADD_SIZE" == name)
    {
        size += data.toULongLong();
        ui->label_size_tip->setText(ConverSize(size)+" （"+QString::number(size)+" 字节）");
    }
    else if("PREPARE_UPLOAD" == name)
    {
        timer.stop();

        ui->label_loading->hide();
        ui->label_complete->show();
    }
    else if("SET_CURR_FILE" == name)
    {
        ui->label_curr_file->setText(data);
    }
    else if("FILE+1" == name)
    {
        uploaded_file_num++;
        ui->progressBarCurrFile->setValue(0);
        ui->label__file_num_progress->setText(QString::number(uploaded_file_num)+" / "+QString::number(file_count));

    }
    else if("UPDATE_PCT" == name)
    {
        int f = data.toFloat()*10;
        if(f >= 1000)
        {
            f = 1000;
        }
        ui->progressBarCurrFile->setValue(f);
    }
    else if("SPEED" == name)
    {
        ui->label__speed->setText(ConverSize(data.toInt())+" 每秒");
    }
    else if("COMPLETE" == name)
    {
        timer_used.stop();
        this->clear_cache_data();
    }
    else
    {}
}

QString DirProcess::ConverSize(unsigned long long bytes)
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


