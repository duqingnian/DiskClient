#pragma execution_character_set("utf-8")
#include "stackpannel.h"
#include <QDebug>

StackPannel::StackPannel(QWidget *parent) : BaseController(parent)
{
    this->setStyleSheet("font-family: \"Microsoft Yahei\";");
    init_info_pannel();
    init_action_pannel();

    chatList = new QWidget(this);
    chatList->setObjectName("chatList");
    chatList->move(0,infoPannel->height());
    chatList->resize(this->width(),300);
    //chatList->setStyleSheet("#chatList{background:red;}");
}

//初始化顶部面板
void StackPannel::init_info_pannel()
{
    infoPannel = new QWidget(this);
    infoPannel->setObjectName("infoPannel");
    infoPannel->move(0,0);
    infoPannel->resize(1000,56);
    infoPannel->setStyleSheet("#infoPannel{background:#fff;border-bottom:1px solid #ECEEF3;}");

    avatar = new Label(infoPannel);
    avatar->setObjectName("avatar");
    avatar->resize(32,32);
    avatar->setScaledContents(true);
    avatar->move(16,12);
    QPixmap map = QPixmap::fromImage(QImage(":/Resources/User/noavatar_small.gif"));
    avatar->setPixmap(map);
    avatar->setStyleSheet("#avatar{border-radius: 10px;}");

    name = new Label(infoPannel);
    name->setObjectName("name");
    name->setText("NAME");
    name->move(62,11);
    name->setMinimumWidth(150);
    name->setStyleSheet("#name{color:#000;font-size:14px;}");

    summary = new Label(infoPannel);
    summary->setObjectName("summary");
    summary->setText("SUMMARY");
    summary->move(62,31);
    summary->setMinimumWidth(400);
    summary->setStyleSheet("#summary{color:#838690;font-size:12px;}");

    info = new Label(infoPannel);
    info->setObjectName("info");
    info->resize(20,20);
    info->setScaledContents(true);
    QPixmap infopx = QPixmap::fromImage(QImage(":/Resources/Chat/info.png"));
    info->setPixmap(infopx);
    info->setCursor(Qt::PointingHandCursor);
}

//初始化动作面板
void StackPannel::init_action_pannel()
{
    actionPannel = new QWidget(this);
    actionPannel->setObjectName("actionPannel");
    //actionPannel->setStyleSheet("#actionPannel{background:red;}");

    action_add = new Label(actionPannel);
    action_add->setObjectName("action_add");
    action_add->resize(26,26);
    action_add->setScaledContents(true);
    QPixmap addpx = QPixmap::fromImage(QImage(":/Resources/Chat/add.png"));
    action_add->setPixmap(addpx);
    action_add->setCursor(Qt::PointingHandCursor);
    action_add->move(13,13);

    action_send = new Label(actionPannel);
    action_send->setObjectName("action_send");
    action_send->resize(26,26);
    action_send->setScaledContents(true);
    QPixmap sendpx = QPixmap::fromImage(QImage(":/Resources/Chat/send.png"));
    action_send->setPixmap(sendpx);
    action_send->setCursor(Qt::PointingHandCursor);
    connect(action_send,&Label::clicked,this,&StackPannel::send_msg);

    input = new QTextEdit(actionPannel);
    input->setObjectName("input");
    input->move(53,8);
    input->setStyleSheet("#input{background:#fff;border-bottom:1px solid #DBDDE0;border-radius:16px;padding:3px 7px;font-size:15px;}");
    connect(input,&QTextEdit::textChanged,this,&StackPannel::resize_input);
}

void StackPannel::set_target(SELECT_UNIT* unit)
{
    target = unit;

    name->setText(unit->unit->name);
    summary->setText(unit->unit->depname+"-"+unit->unit->groupname+"-"+unit->unit->title+"-"+unit->unit->job_number);
}

void StackPannel::resize_input()
{
    QTextDocument *doc = input->document();
    int height = doc->size().height();
    int width = doc->size().width();
    QSize minSize = input->minimumSize();
    if (height < 36)
    {
        height = 36;
    }
    if (width != minSize.width())
    {
        width = this->width() - 100;
    }

    if(height > 250)
    {
        height = 250;
    }

    actionPannel->resize(actionPannel->width(), height + 16);
    actionPannel->move(0,this->height() - actionPannel->height());
    actionPannel->raise();

    action_add->move(13,actionPannel->height() - 39);
    action_send->move(action_send->x(),actionPannel->height() - 39);

    doc->setTextWidth(this->width() - 130);
    input->setFixedHeight(height);

    chatList->resize(this->width(),this->height() - infoPannel->height() - actionPannel->height());
}

//发送聊天信息
void StackPannel::send_msg()
{
    QString msg = input->toPlainText().trimmed();
    if("" != msg)
    {
        sendmsg("zhangsan",msg);
        input->clear();
    }
}

void StackPannel::resizeEvent(QResizeEvent *)
{
    infoPannel->resize(this->width(),56);
    info->move(this->width() - 30,18);

    actionPannel->resize(this->width(),52);
    actionPannel->move(0,this->height() - actionPannel->height());

    action_send->move(this->width() - action_send->width() - 10,13);

    input->resize(this->width() - 100,36);

    chatList->resize(this->width(),this->height() - infoPannel->height() - actionPannel->height());
}
