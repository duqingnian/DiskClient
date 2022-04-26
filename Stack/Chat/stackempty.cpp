#pragma execution_character_set("utf-8")
#include "stackempty.h"

StackEmpty::StackEmpty(QWidget *parent) : BaseController(parent)
{
    start_tip = new QLabel(this);
    start_tip->setText("请选择一个聊天，开始发送信息");
    start_tip->setObjectName("start_tip");
    start_tip->resize(258,38);
    start_tip->setStyleSheet("#start_tip{background:#fff;border-radius: 15px;font-size:16px;border: 1px solid #DBDEE0;}");
    start_tip->setAlignment(Qt::AlignCenter);
}

void StackEmpty::resizeEvent(QResizeEvent *)
{
    start_tip->move((this->width() - start_tip->width())/2,(this->height() - start_tip->height())/2);
}
