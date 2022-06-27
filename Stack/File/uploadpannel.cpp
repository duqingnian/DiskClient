#pragma execution_character_set("utf-8")
#include "uploadpannel.h"
#include "ui_uploadpannel.h"
#include "uploadfilelisteitem.h"
#include <QAbstractItemView>
#include <QDebug>
#include <Thread/syncfilethread.h>
#define DATA_META_LEN 256

UploadPannel::UploadPannel(QWidget *parent):BaseController(parent),ui(new Ui::UploadPannel)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    topbar = new Label(this);
    topbar->setObjectName("topbar");
    topbar->resize(495,38);
    topbar->setStyleSheet("border: 1px solid #0d64b0;background-color: #1361b0;border-top-left-radius: 5px;border-top-right-radius: 5px;");
    topbar->move(0,0);

    connect(topbar,&Label::dbclicked,this,[=](){
        wait(10);
        if("MIN"==show_flag)
        {
            this->onMax();
        }
        else
        {
            this->onMin();
        }
    });

    title = new QLabel(topbar);
    title->setObjectName("title");
    title->setText("上传 (0/0)");
    title->setStyleSheet("#title{color:#fff;font-size:16px;}");
    title->move(13,7);
    title->setMinimumWidth(175);

    //清空
    clear = new Label(topbar);
    clear->setStyleSheet("background:transparent;");
    QImage clearImg(":/Resources/Common/clear.png");
    clear->setPixmap(QPixmap::fromImage(clearImg));
    clear->resize(20,20);
    clear->setScaledContents(true);
    clear->setCursor(QCursor(Qt::PointingHandCursor));
    clear->move(400,9);

    connect(clear,&Label::clicked,this,&UploadPannel::clear_upoload_queue);

    //最小化
    min = new Label(topbar);
    min->setStyleSheet("background:transparent;");
    QImage minImg(":/Resources/Common/min.png");
    min->setPixmap(QPixmap::fromImage(minImg));
    min->resize(20,20);
    min->setScaledContents(true);
    min->setCursor(QCursor(Qt::PointingHandCursor));
    min->move(430,9);

    connect(min,&Label::clicked,this,&UploadPannel::onMin);

    //最大化
    max = new Label(topbar);
    max->setStyleSheet("background:transparent;");
    QImage maxImg(":/Resources/Common/max.png");
    max->setPixmap(QPixmap::fromImage(maxImg));
    max->resize(20,20);
    max->setScaledContents(true);
    max->setCursor(QCursor(Qt::PointingHandCursor));
    max->move(430,9);
    max->hide();

    connect(max,&Label::clicked,this,&UploadPannel::onMax);

    //关闭
    close = new Label(topbar);
    close->setStyleSheet("background:transparent;");
    QImage closeImg(":/Resources/Common/close_w.png");
    close->setPixmap(QPixmap::fromImage(closeImg));
    close->resize(20,20);
    close->setCursor(QCursor(Qt::PointingHandCursor));
    close->move(460,9);

    connect(close,&Label::clicked,this,&UploadPannel::close_upload_pannel);

    QString css = "#upload_file_list{border:0px;background:#fff;}";
    upload_file_list = new QListWidget(this);
    upload_file_list->setObjectName("upload_file_list");
    upload_file_list->setStyleSheet(css);
    upload_file_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    upload_file_list->resize(493,56*6);
    upload_file_list->move(1,38);

    QString file_server_ip   = get_reg("file_server");
    QString file_server_port = get_reg("file_port");

    send_socket = new QTcpSocket();
    send_socket->connectToHost(file_server_ip,file_server_port.toInt());

    if(send_socket->waitForConnected())
    {
        if(send_socket->isOpen())
        {
            this->file_socket_descriptor = send_socket->socketDescriptor();
            Toast::succ("文件服务器连接成功√");

            QDataStream socketStream(send_socket);
            socketStream.setVersion(QDataStream::Qt_5_15);

            QByteArray data;
            data.prepend(QString("BIND_USER:?JOB_NUMBER="+user->job_number).toUtf8());
            data.resize(DATA_META_LEN);
            socketStream << data;
        }
        else
        {
            Toast::err("文件服务器未开启");
        }
    }
    else
    {
        Toast::err("无法连接到文件服务器");
    }
}

void UploadPannel::onMin()
{
    show_flag = "MIN";
    wait(10);
    emit do_some_action("MIN");
    min->hide();
    max->show();
}

void UploadPannel::onMax()
{
    show_flag = "MAX";
    wait(10);
    emit do_some_action("MAX");
    max->hide();
    min->show();
}

void UploadPannel::readyRead()
{
    QByteArray buffer;

    QDataStream socketStream(send_socket);
    socketStream.setVersion(QDataStream::Qt_5_15);
    socketStream.startTransaction();
    socketStream >> buffer;
    send_socket->flush();

    if(!socketStream.commitTransaction())
    {
        return;
    }

    QByteArray data = buffer.mid(DATA_META_LEN);
    QByteArray header = buffer.mid(0,buffer.indexOf(";"));

    if("DQN:?" != header.mid(0,5))
    {
        return;
    }
    QString _header = header;
    //qDebug() << "Get:" << header;
}

void UploadPannel::disconnected()
{

}


void UploadPannel::sync_file_progrrss(QString BUNDLE,QString BUNDLE_ID,QString FD_ID,QString md5, QString state, float pct,unsigned long long SPEED)
{
    if("UPLOADING" == state)
    {
        UploadFileListeItem* upload_file_item = findChild<UploadFileListeItem*>("up"+md5);
        if(upload_file_item)
        {
            upload_file_item->set_progress(pct);
            upload_file_item->set_speed(SPEED);
        }
    }
    else if("UPLOAD_COMPLETE" == state)
    {
        if(pct >= 100)
        {
            upload_queue.removeOne(md5);  //从上传队列中移除

            int complete_count = upload_file_list->count() - upload_queue.count();
            title->setText(QString("上传 (%1/%2)").arg(QString::number(complete_count)).arg(upload_file_list->count()));

            complete_queue.append(md5);
            UploadFileListeItem* upload_file_item = findChild<UploadFileListeItem*>("up"+md5);
            if(upload_file_item)
            {
                upload_file_item->completed();
            }
            wait(10);
            if(complete_count >= upload_file_list->count())
            {
                emit do_some_action("REFRESH");
                wait(10);
            }
            uploading = false;
            this->touch_upload(BUNDLE,BUNDLE_ID.toInt(),FD_ID.toInt(),SJN);  //尝试下次上传
        }
    }
    else
    {}
}


UploadPannel::~UploadPannel()
{
    delete ui;
}

void UploadPannel::add_queue(UP_FILE* upload_file)
{
    if(uploads.count(upload_file->md5) > 0)
    {
        return;
    }

    upload_queue.append(upload_file->md5);
    uploads[upload_file->md5] = upload_file;

    if(UP_STATE::UPLOAD == upload_file->state)
    {
        UploadFileListeItem* upload_file_item = new UploadFileListeItem();
        upload_file_item->setObjectName("up"+upload_file->md5);
        upload_file_item->set_upload_file(upload_file);

        QListWidgetItem* item = new QListWidgetItem(upload_file_list);
        item->setData(Qt::UserRole,upload_file->md5);
        item->setSizeHint(QSize(494,56));

        upload_file_list->setItemWidget(item, upload_file_item);

        title->setText(QString("上传 (%1/%2)").arg(QString::number(uploaded_count)).arg(upload_file_list->count()));
        upload_file_list->setCurrentRow(upload_file_list->count()-1);
    }
    wait(5);
}

//开始上传
void UploadPannel::touch_upload(QString meta_key,int meta_id,int fd_id,QString _sjn)
{
    if(!uploading && upload_queue.count() > 0)
    {
        SJN = _sjn;
        uploading = true;
        this->onMax();

        QString md5 = upload_queue[0];

        FileManager* fm = new FileManager();
        fm->set_socket_descriptor(this->file_socket_descriptor);
        fm->set_file(meta_key,meta_id,fd_id,SJN,uploads[md5]);
        fm->start();

        connect(fm,&FileManager::update_progress,this,&UploadPannel::sync_file_progrrss);
    }
}

void UploadPannel::clear_queue()
{
    uploads.clear();
}

qintptr UploadPannel::GetSocketDescriptor()
{
    return this->file_socket_descriptor;
}

void UploadPannel::_clear_upoload_queue()
{
    QList<QListWidgetItem*> items = upload_file_list->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    foreach(QListWidgetItem * item, items)
    {
        QString complete_md5 = item->data(Qt::UserRole).toString();
        if(-1 != complete_queue.indexOf(complete_md5))
        {
            wait(10);
            delete upload_file_list->takeItem(upload_file_list->row(item));
        }
    }

    wait(10);
    int complete_count = upload_file_list->count() - upload_queue.count();
    title->setText(QString("上传 (%1/%2)").arg(QString::number(complete_count)).arg(upload_file_list->count()));
}

//清空已经上传的列表
void UploadPannel::clear_upoload_queue()
{
    int ret = MSGBOX::alert(this,"请确认","确定清空上传列表吗?",MESSAGE_QUESTION,BUTTON_OK_AND_CANCEL);
    if(BUTTON_OK == ret)
    {
        _clear_upoload_queue();
    }
}

//关闭上传面板
void UploadPannel::close_upload_pannel()
{
    show_flag = "CLOSE";
    _clear_upoload_queue();
    wait(10);
    emit do_some_action("CLOSE");
}
