#include "chatfrienditem.h"
#include "ui_chatfrienditem.h"

#include <Lib/HttpClient.h>

#include <Thread/downloadthread.h>

ChatFriendItem::ChatFriendItem(QWidget *parent) : BaseController(parent),ui(new Ui::ChatFriendItem)
{
    ui->setupUi(this);
    this->setStyleSheet("font-family: \"Microsoft Yahei\";border:0px;QLabel{border:0px;}");
    this->setCursor(Qt::PointingHandCursor);
    ui->last_msg->setText("");
    ui->last_time->setText("");

    ui->bg->lower();
}

ChatFriendItem::~ChatFriendItem()
{
    delete ui;
}

void ChatFriendItem::set_employee(USER* _user)
{
    this->employee = _user;
    ui->label_name->setText(this->employee->name);
    ui->label_avatar->setScaledContents(true);
    if("SYS" == this->employee->job_number)
    {
        ui->label_avatar->setPixmap(QPixmap::fromImage(QImage(":/Resources/ico.ico")));
    }
    else
    {
        ui->label_avatar->setPixmap(QPixmap::fromImage(QImage(":/Resources/User/noavatar_middle.gif")));

        QString avatar_jpg = basepath+"/avatar/"+this->employee->job_number+".jpg";
        QFile avatar_file(avatar_jpg);
        if(!avatar_file.exists())
        {
            if("" != this->employee->avatar && this->employee->avatar.mid(0,4)=="http")
            {
                HttpClient(this->employee->avatar).success([=](const QString &response) {
                    ui->label_avatar->setPixmap(GetAvatar(this->employee->job_number));
                }).download(avatar_jpg);
            }
        }
        else
        {
            ui->label_avatar->setPixmap(GetAvatar(this->employee->job_number));
        }
    }
}

USER* ChatFriendItem::get_employee()
{
    return this->employee;
}

void ChatFriendItem::selected()
{
    ui->bg->setStyleSheet("background:#1DCF67;");
    ui->label_name->setStyleSheet("font-size:14px;color:#fff;");
    ui->last_msg->setStyleSheet("font-size:14px;color:#fff;");
    ui->last_time->setStyleSheet("font-size:14px;color:#fff;");
}


void ChatFriendItem::unselected()
{
    ui->bg->setStyleSheet("background:#fff;");
    ui->label_name->setStyleSheet("font-size:14px;color:#000;");
    ui->last_msg->setStyleSheet("font-size:14px;color:#000;");
    ui->last_time->setStyleSheet("font-size:14px;color:#000;");
}

void ChatFriendItem::set_last_msg(QString last_msg)
{
    ui->last_msg->setText(last_msg);
}

void ChatFriendItem::set_last_time(QString last_time)
{
    ui->last_time->setText(last_time);
}


