﻿#pragma execution_character_set("utf-8")
#include "auth.h"
#include "ui_auth.h"
#include <QPainter>
#include <QStyleOption>
#include <QPainterPath>
#include <QSettings>
#include <QDebug>
#include <QSysInfo>
#include <QJsonParseError>
#include <QJsonObject>
#include <QTimer>
#include <Lib/HttpClient.h>
#include <Component/Msg.h>
#include <QMessageBox>
#include <QMovie>
#include <Thread/downloadthread.h>
#include <Common/db.h>

int border = 8;

Auth::Auth(QWidget *parent) : BaseWindow(parent),ui(new Ui::Auth)
{
    ui->setupUi(this);
    this->m_TitleBar->set_width(this->width());
    this->m_TitleBar->set_bg_color(240,243,245);
    this->m_TitleBar->display_setting(true);

    dlg_setting = new AuthSetting(this);

    if(     "" == get_reg("api_url")  //web请求接口
            || "" == get_reg("socket_server") || "" == get_reg("socket_port") //聊天服务器
            || "" == get_reg("file_server") || "" == get_reg("file_port"))   //文件服务器
    {
        dlg_setting->show();
    }
    else
    {
        if(!Db::Instance()->isOpen())
        {
            box("无法打开数据库");
        }
        else
        {
            //先尝试连接socket
            if(try_connect_server())
            {
                QTcpSocket* socket = Socket::Instance()->handle();
                if(socket->socketDescriptor() != -1)
                {
                    init();
                    init_register();
                }
                else
                {
                    box("socketDescriptor == -1");
                }
            }
            else
            {
                box("无法连接到socket服务器");
            }
        }
    }
    connect(this->m_TitleBar->m_pButtonSetting,&QPushButton::clicked,this,[this]{open_setting_dialog();});
}

void Auth::init()
{
    //淡蓝色背景图
    QWidget* bg = new QWidget(this);
    bg->setObjectName("bg");
    bg->resize(this->width()-border*2,this->height()-(this->m_TitleBar->pos().y() + this->m_TitleBar->height()+border));
    bg->move(border,this->m_TitleBar->pos().y() + this->m_TitleBar->height());
    bg->setStyleSheet("#bg{background-color:#F0F3F5;}");

    //logo
    QLabel* logo = new QLabel(this);
    QImage logo_src(":/Resources/Auth/logo.png");
    logo->setPixmap(QPixmap::fromImage(logo_src));
    logo->move(150,75);

    //登录方式 - 扫码登录
    auth_type_qrcode = new Label(this);
    auth_type_qrcode->setObjectName("auth_type_qrcode");
    auth_type_qrcode->resize(116,55);
    auth_type_qrcode->move(70,160);
    auth_type_qrcode->setStyleSheet("#auth_type_qrcode{background-color:#ffffff;font-size:18px;font-family: \"Microsoft Yahei\";color:#252628;border-top-left-radius: 10px;}");
    auth_type_qrcode->setText("扫码登录");
    auth_type_qrcode->setAlignment(Qt::AlignCenter);
    auth_type_qrcode->setCursor(Qt::PointingHandCursor);
    connect(auth_type_qrcode,  &Label::clicked, this, [this]{ set_auth_type("qrcode"); });

    //登录方式 - 账号密码登录
    auth_type_account = new Label(this);
    auth_type_account->setObjectName("auth_type_qrcode");
    auth_type_account->resize(116,55);
    auth_type_account->move(auth_type_qrcode->x() + auth_type_qrcode->width(),160);
    auth_type_account->setStyleSheet("#auth_type_qrcode{background-color:#F5F5F6;font-family: \"Microsoft Yahei\";font-size:18px;color:#AAABAD;}");
    auth_type_account->setText("账号登录");
    auth_type_account->setAlignment(Qt::AlignCenter);
    auth_type_account->setCursor(Qt::PointingHandCursor);
    connect(auth_type_account,  &Label::clicked, this, [this]{ set_auth_type("account"); });

    //登录方式 - 短信登录
    auth_type_sms = new Label(this);
    auth_type_sms->setObjectName("auth_type_qrcode");
    auth_type_sms->resize(116,55);
    auth_type_sms->move(auth_type_account->x() + auth_type_account->width(),160);
    auth_type_sms->setStyleSheet("#auth_type_qrcode{background-color:#F5F5F6;font-size:18px;font-family: \"Microsoft Yahei\";color:#AAABAD;border-top-right-radius: 10px;}");
    auth_type_sms->setText("短信登录");
    auth_type_sms->setAlignment(Qt::AlignCenter);
    auth_type_sms->setCursor(Qt::PointingHandCursor);
    connect(auth_type_sms,  &Label::clicked, this, [this]{ set_auth_type("sms"); });

    //内容区域
    auth_canvas = new QStackedWidget(this);
    auth_canvas->setObjectName("auth_canvas");
    auth_canvas->resize(116*3,310);
    auth_canvas->move(auth_type_qrcode->x(), auth_type_qrcode->y()+auth_type_qrcode->height());
    auth_canvas->setStyleSheet("#auth_canvas{background-color:#fff;border-bottom-left-radius: 10px;border-bottom-right-radius: 10px;font-family: \"Microsoft Yahei\";}");

    //最底下 提示登录遇到问题
    QLabel* qa_tip = new QLabel(this);
    qa_tip->setObjectName("qa_tip");
    qa_tip->move(0,this->height() - 50);
    qa_tip->resize(this->width(),30);
    qa_tip->setAlignment(Qt::AlignCenter);
    qa_tip->setText("登录遇到问题请钉钉联系技术支持组");
    qa_tip->setStyleSheet("#qa_tip{color:#153160;font-size:12px;}");
}

void Auth::init_register()
{
    _register = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\AdoDisk", QSettings::NativeFormat);
    api_url = _register->value("api_url").toString();
    uid = _register->value("uid").toString();
    if(uid.length() > 20)
    {
        if(uid.mid(0,6) == "ADOSTR" && uid.right(3) == "=E=")
        {
            //uid格式正确，开始网络请求
            HttpClient(api_url+"client/auth/ding/fetch.html").success([this](const QString &response) {
                autologin(response.toUtf8());
            }).param("uid", uid).header("uid", uid).header("token", md5(uid)).header("content-type", "application/x-www-form-urlencoded").post();
        }
        else
        {
            //uid格式不正确，删除各项数据
            _register->setValue("uid","");
            init_stack_widgets();
        }
    }
    else
    {
        init_stack_widgets();
    }
}

bool Auth::try_connect_server()
{
    //淡蓝色背景图
    QWidget* bg = new QWidget(this);
    bg->setObjectName("bg");
    bg->resize(this->width()-border*2,this->height()-(this->m_TitleBar->pos().y() + this->m_TitleBar->height()+border));
    bg->move(border,this->m_TitleBar->pos().y() + this->m_TitleBar->height());
    bg->setStyleSheet("#bg{background-color:#F0F3F5;}");

    //logo
    QLabel* logo = new QLabel(this);
    QImage logo_src(":/Resources/Auth/logo.png");
    logo->setPixmap(QPixmap::fromImage(logo_src));
    logo->move(150,75);

    QLabel* loading_label = new QLabel(this);
    loading_label->resize(160,50);
    loading_label->setScaledContents(true);
    loading_label->move(167,270);

    QMovie* movie = new QMovie(":/Resources/loading.gif");
    loading_label->setMovie(movie);
    movie->start();

    return Socket::Instance()->isOpen();
}


void Auth::autologin(QString ret)
{
    welcome(ret);
}

void Auth::init_stack_widgets()
{
    auth_widget_qrcode  = new AuthQrcode(this);
    auth_widget_account = new AuthAccount(this);
    auth_widget_sms     = new AuthSms(this);

    auth_canvas->addWidget(auth_widget_qrcode);
    auth_canvas->addWidget(auth_widget_account);
    auth_canvas->addWidget(auth_widget_sms);

    //保存配置后重新生成二维码
    connect(dlg_setting,SIGNAL(save_setting()),auth_widget_qrcode,SLOT(refresh_qrcode()));
    connect(this,&Auth::login_success,auth_widget_qrcode,&AuthQrcode::pause);

    connect(auth_widget_qrcode,&AuthQrcode::open_welcome,this,&Auth::welcome);
    connect(auth_widget_account,&AuthAccount::open_welcome,this,&Auth::welcome);
}

void Auth::render_auth_component()
{
    auth_widget_qrcode->hide();
    auth_widget_account->hide();
    auth_widget_sms->hide();

    auth_type_qrcode->setStyleSheet("#auth_type_qrcode{background-color:#F5F5F6;font-size:18px;color:#AAABAD;border-top-left-radius: 10px;font-family: \"Microsoft Yahei\";}");
    auth_type_account->setStyleSheet("#auth_type_qrcode{background-color:#F5F5F6;font-size:18px;color:#AAABAD;font-family: \"Microsoft Yahei\";}");
    auth_type_sms->setStyleSheet("#auth_type_qrcode{background-color:#F5F5F6;font-size:18px;color:#AAABAD;border-top-right-radius: 10px;font-family: \"Microsoft Yahei\";}");

    if("qrcode" == auth_type)
    {
        auth_type_qrcode->setStyleSheet("#auth_type_qrcode{background-color:#ffffff;font-size:18px;color:#252628;border-top-left-radius: 10px;font-family: \"Microsoft Yahei\";}");
        auth_canvas->setCurrentWidget(auth_widget_qrcode);
        auth_widget_qrcode->show();
    }
    else if("account" == auth_type)
    {
        auth_type_account->setStyleSheet("#auth_type_qrcode{background-color:#ffffff;font-size:18px;color:#252628;font-family: \"Microsoft Yahei\";}");
        auth_canvas->setCurrentWidget(auth_widget_account);
        auth_widget_account->show();
    }
    else if("sms" == auth_type)
    {
        auth_type_sms->setStyleSheet("#auth_type_qrcode{background-color:#ffffff;font-size:18px;color:#252628;border-top-right-radius: 10px;font-family: \"Microsoft Yahei\";}");
        auth_canvas->setCurrentWidget(auth_widget_sms);
        auth_widget_sms->show();
    }
    else
    {
        //do nothing
    }
}

void Auth::set_auth_type(QString _auth_type)
{
    auth_type = _auth_type;
    render_auth_component();
}

//打开配置弹窗
void Auth::open_setting_dialog()
{
    dlg_setting->exec();
}

void Auth::welcome(QString res)
{
    QJsonParseError err_rpt;
    QJsonDocument  jsonDoc = QJsonDocument::fromJson(res.toLatin1(), &err_rpt);
    if(err_rpt.error == QJsonParseError::NoError)
    {
        QJsonObject rootObj = jsonDoc.object();
        if(rootObj.contains("code"))
        {
            QJsonValue _code = rootObj.value("code");
            int code = _code.toInt();
            if(0 == code)
            {
                QJsonObject userObj = rootObj.value("user").toObject();

                user->uid = userObj.value("uid").toString();
                user->name = userObj.value("name").toString();
                user->avatar = userObj.value("avatar").toString();
                user->depid = userObj.value("depid").toString();
                user->depname = userObj.value("depname").toString();
                user->groupid = userObj.value("groupid").toString();
                user->groupname = userObj.value("groupname").toString();
                user->title = userObj.value("title").toString();
                user->job_number = userObj.value("job_number").toString().toLower();

                _register->setValue("uid",user->uid);
                sendJsonObject("SYS",userObj,"SYNC_INFO");
                //尝试头像
                //qDebug() << "QSslSocket::sslLibraryBuildVersionString()" << QSslSocket::sslLibraryBuildVersionString();
                //qDebug() << "QSslSocket::sslLibraryVersionString()" << QSslSocket::sslLibraryVersionString();
                //qDebug() << "Supports SSL: " << QSslSocket::supportsSsl();

                QString avatar_jpg = basepath+"/avatar/"+user->job_number.toUpper()+".jpg";
                QFile avatar_file(avatar_jpg);
                if(!avatar_file.exists())
                {
                    if("" != user->avatar && user->avatar.mid(0,4)=="http")
                    {
                        //下载头像
                        //DownLoadThread* t_down = new DownLoadThread(user->avatar,avatar_jpg);
                        //t_down->start();
                        HttpClient(user->avatar).success([=](const QString &response) {
                            emit login_success();
                            accept();
                        }).download(avatar_jpg);
                    }
                }
                emit login_success();
                accept();
            }
            else
            {
                init_stack_widgets();
            }
        }
    }
    else{
        init_stack_widgets();
    }
}

Auth::~Auth()
{
    delete ui;
}

void Auth::closeEvent(QCloseEvent *event)
{

}
