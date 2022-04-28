#ifndef STACKPANNEL_H
#define STACKPANNEL_H

#include "bubblelist.h"

#include <QWidget>
#include <Common/basecontroller.h>
#include <QResizeEvent>
#include <Component/Label.h>
#include <QScrollArea>
#include <QTextEdit>

class StackPannel : public BaseController
{
    Q_OBJECT
public:
    explicit StackPannel(QWidget *parent = nullptr,SELECT_UNIT* unit = nullptr);
    void resizeEvent(QResizeEvent *) override;

    //初始化顶部面板
    void init_info_pannel();
    void init_action_pannel();
    void init_chat_list();
    void set_target(SELECT_UNIT*);
    void sendTimeMsg(qint64 time, QString msg);
private:
    QWidget* infoPannel;
    Label* avatar;
    Label* name;
    Label* summary;
    Label* info;
    QWidget* actionPannel;
    SELECT_UNIT* target;
    Label* action_send;
    Label* action_add;
    QTextEdit* input;
    QScrollArea* scrollArea;
    BubbleList *bubble_list;
    qint64 lastMsgTime = 0;
private slots:
    void resize_input();
    void send_msg();
};

#endif // STACKPANNEL_H
