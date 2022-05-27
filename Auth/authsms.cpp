#pragma execution_character_set("utf-8")
#include "authsms.h"
#include "ui_authsms.h"

AuthSms::AuthSms(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuthSms)
{
    ui->setupUi(this);
    init();
}

void AuthSms::init()
{
    edit_account = new QLineEdit(this);
    edit_account->setObjectName("edit_account");
    edit_account->resize(305,50);
    edit_account->setPlaceholderText("请输入手机号码");
    edit_account->move(25,20);
    edit_account->setStyleSheet("#edit_account{background-color:#f7f7f7;color:#565656;border:0px;border-radius:10px;font-size:14px;font-family: \"Microsoft Yahei\";padding-left:10px;}");

    edit_password = new QLineEdit(this);
    edit_password->setObjectName("edit_password");
    edit_password->resize(305,50);
    edit_password->setPlaceholderText("请输入验证码");
    edit_password->move(25,80);
    edit_password->setStyleSheet("#edit_password{background-color:#f7f7f7;color:#565656;border:0px;border-radius:10px;font-size:14px;font-family: \"Microsoft Yahei\";padding-left:10px;}");

    //获取验证码
    btn_getcode = new QPushButton(edit_password);
    btn_getcode->setObjectName("btn_getcode");
    btn_getcode->resize(100,30);
    btn_getcode->setText("获取验证码");
    btn_getcode->move(195,10);
    btn_getcode->setCursor(Qt::PointingHandCursor);
    btn_getcode->setStyleSheet("#btn_getcode{color:#7B93FF;border:1px solid #7B93FF;border-radius:4px;font-size:14px;font-family: \"Microsoft Yahei\";text-align:center;}");
    btn_getcode->raise();

    //提交按钮
    btn_submit = new QPushButton(this);
    btn_submit->setObjectName("btn_submit");
    btn_submit->resize(305,50);
    btn_submit->setText("登录");
    btn_submit->move(25,155);
    btn_submit->setCursor(Qt::PointingHandCursor);
    btn_submit->setStyleSheet("#btn_submit{background-color:#7B93FF;color:#fff;border:0px;border-radius:10px;font-size:16px;font-family: \"Microsoft Yahei\";text-align:center;}");

    //自动登录
    auto_check = new Label(this);
    auto_check->setObjectName("auto_check");
    auto_check->move(25,220);
    auto_check->resize(20,20);
    auto_check->setStyleSheet("#auto_check{border-image:url(:/Resources/Form/listhead_check.png) 0 0 0 20;}");
    auto_check->setCursor(Qt::PointingHandCursor);
    connect(auto_check,&Label::clicked,this,&AuthSms::set_remember);

    auto_txt = new Label(this);
    auto_txt->setText("下次自动登录");
    auto_txt->setObjectName("auto_txt");
    auto_txt->move(45,220);
    auto_txt->setCursor(Qt::PointingHandCursor);
    auto_txt->setStyleSheet("#auto_txt{font-size:12px;color:#B0B1B3;font-family: \"Microsoft Yahei\";}");
    connect(auto_txt,&Label::clicked,this,&AuthSms::set_remember);
}


void AuthSms::set_remember()
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

AuthSms::~AuthSms()
{
    delete ui;
}
