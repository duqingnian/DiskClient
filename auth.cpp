#pragma execution_character_set("utf-8")
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

bool DEBUG = false;
int border = 8;

Auth::Auth(QWidget *parent) : BaseWindow(parent),ui(new Ui::Auth)
{
    ui->setupUi(this);
    this->m_TitleBar->set_width(this->width());
    this->m_TitleBar->set_bg_color(240,243,245);
    this->m_TitleBar->display_setting(true);

    dlg_setting = new AuthSetting(this);
    connect(this->m_TitleBar->m_pButtonSetting,&QPushButton::clicked,this,[this]{open_setting_dialog();});

    init();
    init_stack_widgets();
    loading();

    if(DEBUG)
    {
        QTimer::singleShot(1000,this,[=](){
            checkLater();
        });
    }
    else
    {
        checkLater();
    }
}

void Auth::checkLater()
{
    _register = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\AdoDisk", QSettings::NativeFormat);

    //下面是必要的检查
    bool app_env = true;

    QString socket_server   = get_reg("socket_server");
    QString socket_port = get_reg("socket_port");
    QString file_server = get_reg("file_server");
    QString file_port   = get_reg("file_port");

    // 1.配置是不是正确
    if("" == get_reg("api_url")|| "" == socket_server || "" == socket_port || "" == file_server || "" == file_port)
    {
        app_env = false;
        loading_label->hide();
        error_label->setText("ERROR: 配置信息丢失，请先配置");
    }

    // 2.消息socket
    //QTcpSocket* msg_test = new QTcpSocket(this);
    //msg_test->connectToHost(socket_server,socket_port.toInt());
    /*
    if(msg_test->waitForConnected())
    {
        if(msg_test->isOpen())
        {
            if(!sendmsg("SYS","PING"))
            {
                app_env = false;
                loading_label->hide();
                error_label->setText("ERROR: 消息服务器未开启，错误码：60063");
            }
            else
            {
                 //qDebug() << "ping msg server OK";
            }
        }
        else
        {
            app_env = false;
            loading_label->hide();
            error_label->setText("ERROR: 消息服务器未开启，错误码：60070");
        }
    }
    else
    {
        app_env = false;
        loading_label->hide();
        error_label->setText("ERROR: 无法链接到消息服务器，错误码：60076");
    }
    msg_test->abort();
    msg_test->deleteLater();
    */
    // 3.文件socket
    // 4.连接mysql数据库
    if(!Db::Instance()->isOpen())
    {
        app_env = false;
        loading_label->hide();
        error_label->setText("ERROR: 无法连接到数据库");
    }
    // 5.是不是需要自动登录
    QString uid = get_reg("uid");


    if(DEBUG)
    {
        user->uid = "ADOSTR27ddgmBnR924aJLUC3QRlqqGCJF-ipb1x_433ZipmlmGNfXqPdfvmduwOZRyKJVz4FI7=E=";
        user->name = "阿杜调试";
        user->avatar = "";
        user->depid = "1001";
        user->depname = "软件测试部";
        user->groupid = "1002";
        user->groupname = "测试调试组";
        user->title = "技术总监";
        user->job_number = "CZML669";

        //emit login_success();
        wait(1000);
        QThread::sleep(1);
        accept();
    }
    else
    {
        if(app_env)
        {
            if(uid.length() > 10)
            {
                //自动登录
                if(uid.mid(0,6) == "ADOSTR" && uid.right(3) == "=E=")
                {
                    loading();
                    QString api_url = get_reg("api_url");
                    HttpClient(api_url+"client/auth/ding/fetch.html").success([this](const QString &response) {
                        autologin(response.toUtf8());
                    }).param("uid", uid).header("uid", uid).header("token", md5(uid)).header("content-type", "application/x-www-form-urlencoded").post();
                }
                else
                {
                    //uid格式不正确，删除各项数据
                    _register->setValue("uid","");
                }
            }
            loading_widget->hide();
        }
    }
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

void Auth::loading()
{
    loading_widget = new QWidget(this);
    loading_widget->setObjectName("loading_widget");
    loading_widget->resize(this->width()-border*2,this->height()-(this->m_TitleBar->pos().y() + this->m_TitleBar->height()+border));
    loading_widget->move(border,this->m_TitleBar->pos().y() + this->m_TitleBar->height());
    loading_widget->setStyleSheet("#loading_widget{background-color:#F0F3F5;}");

    //logo
    QLabel* logo = new QLabel(loading_widget);
    QImage logo_src(":/Resources/Auth/logo.png");
    logo->setPixmap(QPixmap::fromImage(logo_src));
    logo->move(150,75);

    loading_label = new QLabel(loading_widget);
    loading_label->resize(60,60);
    loading_label->setScaledContents(true);
    loading_label->move( (this->width() - 60)/2 ,(this->height() - 60)/2);

    error_label = new QLabel(loading_widget);
    error_label->setObjectName("error_label");
    error_label->resize(470,60);
    error_label->move( 0 ,(this->height() - 60)/2);
    error_label->setStyleSheet("#error_label{color:red;font-size:18px;font-family: \"Microsoft Yahei\";}");
    error_label->setAlignment(Qt::AlignHCenter);

    QMovie* movie = new QMovie(":/Resources/load.gif");
    loading_label->setMovie(movie);
    movie->start();
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
    //connect(dlg_setting,SIGNAL(save_setting()),auth_widget_qrcode,SLOT(refresh_qrcode()));
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
                user->job_number = userObj.value("job_number").toString().toUpper();

                _register->setValue("uid",user->uid);

                //请求网络
                QString api_url   = get_reg("api_url");
                HttpClient(api_url+"client/auth/ding/check.html").success([this](const QString &response) {
                    emit login_success();
                    accept();
                }).param("uid", user->uid).param("res", res).header("uid", user->uid).header("token", md5(user->uid)).header("content-type", "application/x-www-form-urlencoded").post();
            }
            else
            {
                loading_widget->hide();
                box("ERROR: 登录系统失败！CODE="+_code.toString());
            }
        }
        else
        {
            loading_widget->hide();
            box("登录返回值不包含code!"+res);
        }
    }
    else{
        box("QJsonParseError::NoError");
    }
}

Auth::~Auth()
{
    delete ui;
}

void Auth::closeEvent(QCloseEvent *event)
{

}
