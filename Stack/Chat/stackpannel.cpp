#pragma execution_character_set("utf-8")
#include "stackpannel.h"
#include <QDebug>

StackPannel::StackPannel(QWidget *parent) : BaseController(parent)
{
    this->setStyleSheet("font-family: \"Microsoft Yahei\";");
    init_info_pannel();
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
    name->setStyleSheet("#name{color:#000;font-size:14px;}");

    summary = new Label(infoPannel);
    summary->setObjectName("summary");
    summary->setText("SUMMARY");
    summary->move(62,31);
    summary->setStyleSheet("#summary{color:#838690;font-size:12px;}");

    info = new Label(infoPannel);
    info->setObjectName("info");
    info->resize(20,20);
    info->setScaledContents(true);
    QPixmap infopx = QPixmap::fromImage(QImage(":/Resources/Chat/info.png"));
    info->setPixmap(infopx);
    info->setCursor(Qt::PointingHandCursor);
}

void StackPannel::set_target(SELECT_UNIT* unit)
{
    target = unit;

    name->setText(unit->unit->name);
    summary->setText(unit->unit->depname+"-"+unit->unit->groupname+"-"+unit->unit->title+"-"+unit->unit->job_number);
}

void StackPannel::resizeEvent(QResizeEvent *)
{
    infoPannel->resize(this->width(),56);
    info->move(this->width() - 30,18);
}
