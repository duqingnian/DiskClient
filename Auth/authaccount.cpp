#pragma execution_character_set("utf-8")
#include "authaccount.h"
#include "ui_authaccount.h"

#include <Lib/HttpClient.h>

#include <QJsonDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>


AuthAccount::AuthAccount(QWidget *parent) : BaseController(parent) , ui(new Ui::AuthAccount)
{
    ui->setupUi(this);
    init();
}

AuthAccount::~AuthAccount()
{
    delete ui;
}

void AuthAccount::init()
{
    edit_account = new QLineEdit(this);
    edit_account->setObjectName("edit_account");
    edit_account->resize(305,50);
    edit_account->setPlaceholderText("请输入账号");
    edit_account->move(25,20);
    edit_account->setStyleSheet("#edit_account{background-color:#f7f7f7;color:#565656;border:0px;border-radius:10px;font-size:14px;font-family: \"Microsoft Yahei\";padding-left:10px;}");

    edit_password = new QLineEdit(this);
    edit_password->setObjectName("edit_password");
    edit_password->setEchoMode(QLineEdit::Password);
    edit_password->resize(305,50);
    edit_password->setPlaceholderText("登录密码");
    edit_password->move(25,80);
    edit_password->setStyleSheet("#edit_password{background-color:#f7f7f7;color:#565656;border:0px;border-radius:10px;font-size:14px;font-family: \"Microsoft Yahei\";padding-left:10px;}");

    //提交按钮
    btn_submit = new QPushButton(this);
    btn_submit->setObjectName("btn_submit");
    btn_submit->resize(305,50);
    btn_submit->setText("登录");
    btn_submit->move(25,155);
    btn_submit->setCursor(Qt::PointingHandCursor);
    btn_submit->setStyleSheet("#btn_submit{background-color:#7B93FF;color:#fff;border:0px;border-radius:10px;font-size:16px;font-family: \"Microsoft Yahei\";text-align:center;}");
    connect(btn_submit,&QPushButton::clicked,this,[=](){
        do_login();
    });

    //自动登录
    auto_check = new Label(this);
    auto_check->setObjectName("auto_check");
    auto_check->move(25,220);
    auto_check->resize(20,20);
    auto_check->setStyleSheet("#auto_check{border-image:url(:/Resources/Form/listhead_check.png) 0 0 0 20;}");
    auto_check->setCursor(Qt::PointingHandCursor);
    connect(auto_check,&Label::clicked,this,&AuthAccount::set_remember);

    auto_txt = new Label(this);
    auto_txt->setText("下次自动登录");
    auto_txt->setObjectName("auto_txt");
    auto_txt->move(45,220);
    auto_txt->setCursor(Qt::PointingHandCursor);
    auto_txt->setStyleSheet("#auto_txt{font-size:12px;color:#B0B1B3;font-family: \"Microsoft Yahei\";}");
    connect(auto_txt,&Label::clicked,this,&AuthAccount::set_remember);

    //忘记密码
    Label* label_forget_passwd = new Label(this);
    label_forget_passwd->setText("忘记密码?");
    label_forget_passwd->move(265,220);
    label_forget_passwd->setStyleSheet("color:#333;font-size:12px;font-family: \"Microsoft Yahei\";");
}

void AuthAccount::do_login()
{
    QString username = edit_account->text();
    QString password = edit_password->text();

    if(username.length() < 2)
    {
        box("请输入账号");
    }
    else if(password.length() < 2)
    {
        box("请输入密码");
    }
    else
    {
        QString rand = random(8);
        int ts = get_time();
        QString token = md5("DUQINGNIAN10985-"+username+rand); //qDebug() << "DUQINGNIAN10985-"+username+rand << " , token=" << token<< " , rand=" << rand;
        //token = token+QString(ts);
        //token = md5(token);

        QSettings* _register = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\AdoDisk", QSettings::NativeFormat);
        QString api_url =  _register->value("api_url").toString();qDebug() << api_url+"client/auth/login.form";
        HttpClient(api_url+"client/auth/login.form").success([=](const QString &response) {
            QJsonParseError err_rpt;
            QJsonDocument  jsonDoc = QJsonDocument::fromJson(response.toUtf8(), &err_rpt);
            if(err_rpt.error != QJsonParseError::NoError)
            {
                QMessageBox msgBox;
                msgBox.setText("登录时服务器返回了错误的数据格式");
                msgBox.exec();
            }
            else
            {
                QJsonObject rootObj = jsonDoc.object();
                if(!rootObj.contains("code"))
                {
                    QMessageBox msgBox;
                    msgBox.setText("服务器未返回code");
                    msgBox.exec();
                }
                else
                {
                    QJsonValue _code = rootObj.value("code");
                    int code = _code.toInt();
                    if(0 == code)
                    {
                        emit open_welcome(response.toUtf8());
                    }
                    else
                    {
                        QJsonValue _msg = rootObj.value("msg");
                        QString msg = _msg.toString();

                        QMessageBox msgBox;
                        msgBox.setText(msg);
                        msgBox.exec();
                    }
                }
            }
        })
                .param("username", username).param("password", password)
                .header("rand", rand)
                .header("ts", QString::number(ts))
                .header("token", token)
                .header("content-type", "application/x-www-form-urlencoded").post();
    }
}


void AuthAccount::set_remember()
{
    if(remember)
    {
        remember = false;
        auto_check->setStyleSheet("#auto_check{border-image:url(:/Resources/Form/listhead_check.png) 0 20 0 0;}");
    }
    else
    {
        remember = true;
        auto_check->setStyleSheet("#auto_check{border-image:url(:/Resources/Form/listhead_check.png) 0 0 0 20;}");
    }
}






