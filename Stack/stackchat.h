#ifndef STACKCHAT_H
#define STACKCHAT_H

#include "Common/basecontroller.h"
#include <QEvent>
#include <Component/Label.h>
#include <QStackedWidget>
#include <Stack/Chat/chat.h>
#include <Stack/Chat/stackempty.h>
#include <Stack/Chat/stackpannel.h>

struct SIDE_TAB{
    QString key = "";
    QString txt = "";
    QString ico = "";
};

class StackChat : public BaseController
{
    Q_OBJECT
public:
    explicit StackChat(QWidget *parent = nullptr);

    //全局方法
    void resizeEvent(QResizeEvent *) override;
    bool eventFilter(QObject*,QEvent*) override;

    //侧边栏方法
    void initSide();
    void renderSide();
    void SelectedTab(QString);

    void HoverEnter(QObject*);
    void HoverLeave(QObject*);

    //内容区域
    void initMain();
    void renderMain();

private:
    QTcpSocket* socket;
    //侧边栏
    QWidget* side;
    QStackedWidget* StackSide;

    //TAB
    QWidget* tab;
    QMap<int,SIDE_TAB*>side_tabs;
    QString selected_tab = "";


    //侧边栏Stacks
    Chat* chat;

    //主面板
    QWidget* main;
    QStackedWidget* StackMain;
    QMap<QString,StackPannel*> chatPannelList;

    //主面板Stacks
    StackEmpty*  stack_empty;

    //当前选中的好友
    SELECT_UNIT* selected_unit;

    void friend_targeted(SELECT_UNIT*);

    //新消息
    void new_message();

    //分发消息
    void dispatch_msg();

    //初始化文件助手
    void init_file_helper();

private slots:


};

#endif // STACKCHAT_H




