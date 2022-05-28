#pragma execution_character_set("utf-8")
#include "uploadpannel.h"
#include "ui_uploadpannel.h"
#include "uploadfilelisteitem.h"
#include <QAbstractItemView>
#include <QDebug>

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

void UploadPannel::sync_file_progrrss(QString BUNDLE,QString BUNDLE_ID,QString FD_ID,QString md5, QString state, float pct)
{
    //qDebug() << "state=" << state;
    if("SYNC_UP_STATE" == state)
    {
        UploadFileListeItem* upload_file_item = findChild<UploadFileListeItem*>("up"+md5);
        upload_file_item->set_progress(pct);
        if(pct >= 100)
        {
            upload_queue.removeOne(md5);  //从上传队列中移除

            int complete_count = upload_file_list->count() - upload_queue.count();
            title->setText(QString("上传 (%1/%2)").arg(QString::number(complete_count)).arg(upload_file_list->count()));

            complete_queue.append(md5);
            UploadFileListeItem* upload_file_item = findChild<UploadFileListeItem*>("up"+md5);
            upload_file_item->completed();

            wait(10);
            uploading = false;
            this->touch_upload(BUNDLE,BUNDLE_ID.toInt(),FD_ID.toInt());  //尝试下次上传
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

    UploadFileListeItem* upload_file_item = new UploadFileListeItem();
    upload_file_item->setObjectName("up"+upload_file->md5);
    upload_file_item->set_upload_file(upload_file);

    QListWidgetItem* item = new QListWidgetItem(upload_file_list);
    item->setData(Qt::UserRole,upload_file->md5);
    item->setSizeHint(QSize(494,56));

    upload_file_list->setItemWidget(item, upload_file_item);

    title->setText(QString("上传 (%1/%2)").arg(QString::number(uploaded_count)).arg(upload_file_list->count()));
    upload_file_list->setCurrentRow(upload_file_list->count()-1);

    wait(10);
}

//开始上传
void UploadPannel::touch_upload(QString meta_key,int meta_id,int fd_id)
{
    if(!uploading && upload_queue.count() > 0)
    {
        uploading = true;
        this->onMax();

        QString md5 = upload_queue[0];

        FileManager* fm = new FileManager();
        fm->set_socket_descriptor(this->file_socket_descriptor);
        fm->set_file(meta_key,meta_id,fd_id,uploads[md5]);
        fm->start();
    }
}

void UploadPannel::set_descriptor(qintptr descriptor)
{
    this->file_socket_descriptor = descriptor;
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

void UploadPannel::sync_file(QString md5)
{
    //    HttpClient(path("client/file.sync")).success([=](const QString &response) {
    //        QJsonParseError err_rpt;
    //        QJsonDocument  jsonDoc = QJsonDocument::fromJson(response.toLatin1(), &err_rpt);
    //        if(err_rpt.error == QJsonParseError::NoError)
    //        {
    //            QJsonObject rootObj = jsonDoc.object();
    //            if(rootObj.contains("code"))
    //            {
    //                QJsonValue _code = rootObj.value("code");
    //                int code = _code.toInt();
    //                if(0 == code){
    //                    complete_queue.append(md5);   //添加到已经上传的集合里面
    //                    UploadFileListeItem* upload_file_item = findChild<UploadFileListeItem*>("up"+md5);
    //                    upload_file_item->completed();

    //                    if(upload_queue.count() < 1)
    //                    {
    //                        //emit do_some_action("REFRESH");
    //                    }
    //                }else{
    //                    MSGBOX::error(this,"文件同步出错了!code="+QString::number(code));
    //                }
    //            }else{
    //                MSGBOX::error(this,"Response don't contains code!");
    //            }
    //        }else{
    //            MSGBOX::error(this,"QJsonParseError!文件同步出错了!");
    //        }
    //    })
    //            .header("content-type", "application/x-www-form-urlencoded")
    //            .param("uid", user->uid)
    //            .param("file_real_name",md5+"."+uploads[md5]->suffix)
    //            .param("file_suffix",uploads[md5]->suffix)
    //            .param("file_original_name",uploads[md5]->name)
    //            .param("file_size",uploads[md5]->size)
    //            .param("fd_id", fd->id)
    //            .post();
}

//void UploadPannel::readyRead()
//{
//    QByteArray buffer;
//    buffer = s->readAll();

//    QString data = QString::fromStdString(buffer.toStdString());
//    QList<QString> datas = data.split("DQN|");

//    for (int i=0; i<datas.count(); i++) {
//        if(datas[i].contains("MD5:"))
//        {
//            QString msg = datas[i].mid(0,datas[i].indexOf(";"));

//            QString META ="";
//            QString MD5 ="";
//            QString STATE ="";
//            float PCT = 0.0f;

//            QStringList headers = msg.split(",");
//            for(QString hi : headers)
//            {
//                QStringList his = hi.split(":");
//                if("MD5" == his[0])
//                {
//                    MD5 = his[1];
//                }
//                else if("META" == his[0])
//                {
//                    META = his[1].toUpper();
//                }
//                else if("STATE" == his[0])
//                {
//                    STATE = his[1].toUpper();
//                }
//                else if("PCT" == his[0])
//                {
//                    PCT = his[1].toFloat();
//                }
//                else{}
//            }
//            //更新文件状态
//            if("SYNC_UP_STATE" == META)
//            {
//                UploadFileListeItem* upload_file_item = findChild<UploadFileListeItem*>("up"+MD5);
//                upload_file_item->set_progress(PCT);
//            }

//            //上传到100%后并不是上传完成了，需要录入数据库后才发送UPLOAD_SUCCESS这个信号
//            if("UPLOAD_SUCCESS" == META)
//            {
//                upload_queue.removeOne(MD5);  //从上传队列中移除

//                int complete_count = upload_file_list->count() - upload_queue.count();
//                title->setText(QString("上传 (%1/%2)").arg(QString::number(complete_count)).arg(upload_file_list->count()));

//                complete_queue.append(MD5);
//                UploadFileListeItem* upload_file_item = findChild<UploadFileListeItem*>("up"+MD5);
//                upload_file_item->completed();

//                wait(10);
//                uploading = false;
//                this->touch_upload();  //尝试下次上传
//            }
//        }
//    }
//}

//void UploadPannel::disconnected(){}
