#pragma execution_character_set("utf-8")
#include "chat.h"

Chat::Chat(QWidget *parent) : BaseController(parent)
{
    QWidget* w1 = new QWidget(this);
    w1->resize(100,100);
    w1->setStyleSheet("background:yellow");
    w1->move(10,70);

    QLabel* l = new QLabel(this);
    l->setText("部门");
    l->move(10,10);

}
