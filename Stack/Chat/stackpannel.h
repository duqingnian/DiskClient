#ifndef STACKPANNEL_H
#define STACKPANNEL_H

#include <QWidget>
#include <Common/basecontroller.h>
#include <QResizeEvent>
#include <Component/Label.h>
#include <QListWidget>
#include <QScrollArea>
#include <QTextEdit>
#include "qnchatmessage.h"
#include <QPaintEvent>

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
    void dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time, QNChatMessage::User_Type type);
    void dealMessageTime(QString curMsgTime);
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void new_message(QString type,QString msg);
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
    qint64 lastMsgTime = 0;
    QListWidget* bubblelist;
    bool isSending = true;
    bool key_control = false;
private slots:
    void resize_input();
    void send_msg();
    void reset_input();
};

#endif // STACKPANNEL_H
