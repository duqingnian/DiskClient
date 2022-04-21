#pragma execution_character_set("utf-8")
#include "stackchat.h"

StackChat::StackChat(QWidget *parent) : BaseController(parent)
{
    QWidget* aa = new QWidget(this);
    aa->resize(300,200);
    aa->move(20,30);
    aa->setStyleSheet("background:green;");
}

void StackChat::resizeEvent(QResizeEvent *)
{

}
