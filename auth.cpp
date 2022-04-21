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
#include <Lib/HttpClient.h>
#include <Component/Msg.h>

Auth::Auth(QWidget *parent) : BaseWindow(parent),ui(new Ui::Auth)
{
    ui->setupUi(this);
    this->m_TitleBar->set_width(this->width());
    this->m_TitleBar->set_bg_color(236,239,255);
    this->m_TitleBar->display_setting(true);

    dlg_setting = new AuthSetting(this);
    init();
    init_register();

    connect(this->m_TitleBar->m_pButtonSetting,&QPushButton::clicked,this,[this]{open_setting_dialog();});
}

void Auth::init()
{
    int border = 8;
    //淡蓝色背景图
    QWidget* bg = new QWidget(this);
    bg->setObjectName("bg");
    bg->resize(this->width()-border*2,this->height()-(this->m_TitleBar->pos().y() + this->m_TitleBar->height()+border));
    bg->move(border,this->m_TitleBar->pos().y() + this->m_TitleBar->height());
    bg->setStyleSheet("#bg{background-color:#ECEFFF;}");

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

/**
 * 注册表中保存的信息：
 * 1.二维码生成的url地址
 * 2.socket服务器的地址
 * 3.cpuid+硬盘id+网卡id的md5
 * 4.用户信息
 *      4.1 钉钉userid
 *      4.2 微信openid
 *      4.3 部门id
 *      4.4 部门名称
 * 5.是否自动登录
 * 6.上次登录的时间
 */
void Auth::init_register()
{
    _register = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\AdoDisk", QSettings::NativeFormat);
    api_url = _register->value("api_url").toString();
    socket_server = _register->value("socket_server").toString();
    machineUniqueId = _register->value("machineUniqueId").toString();
    //如果注册表里的电脑ID不等于计算出来的id，就弹出设置，让用户设置
    if(QString(QSysInfo::machineUniqueId()) != machineUniqueId)
    {
        dlg_setting->show();
    }
    else
    {
        bool connect_ok = false;
        //如果socket设置为空或者不合理，也提示
        QString socket_server = _register->value("socket_server").toString();
        QString socket_port = _register->value("socket_port").toString();
        if("" == api_url || "" == socket_server || "" == socket_port)
        {
            dlg_setting->show();
        }
        else
        {
            socket->connectToHost(server_ip,server_port.toInt());
            if(socket->waitForConnected())
            {
                qDebug() << "Connected to Server["+server_ip+"]["+server_port+"]";
                connect_ok = true;
            }
            else{
                box(QString("socket连接失败了!  %1.").arg(socket->errorString()));
            }
        }
        if(connect_ok)
        {
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
        else
        {

            box("无法连接到socket服务端,请检查服务端是否开启.");
        }
    }
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
    connect(auth_widget_qrcode,&AuthQrcode::open_welcome,this,&Auth::welcome);
    connect(this,&Auth::login_success,auth_widget_qrcode,&AuthQrcode::pause);
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
    qDebug() << "res=" << res;
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
                user->depid = userObj.value("depid").toString();
                user->depname = userObj.value("depname").toString();
                user->groupid = userObj.value("groupid").toString();
                user->groupname = userObj.value("groupname").toString();
                user->title = userObj.value("title").toString();
                user->job_number = userObj.value("job_number").toString();

                if(socket->isOpen())
                {
                    //socket发送普通消息
                    /*
                    QString str = "登录成功|"+user->name+"|"+user->depname+"|"+user->job_number;

                    QDataStream socketStream(socket);
                    socketStream.setVersion(QDataStream::Qt_5_12);

                    QByteArray header;
                    header.prepend(QString("fileType:message,fileName:null,fileSize:%1;").arg(str.size()).toUtf8());
                    header.resize(128);

                    QByteArray byteArray = str.toUtf8();
                    byteArray.prepend(header);

                    socketStream << byteArray;
                    */

                    _register->setValue("uid",user->uid);
                    emit login_success();
                    accept();
                }
                else
                {
                    box("无法连接到socket服务器！");
                }
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
