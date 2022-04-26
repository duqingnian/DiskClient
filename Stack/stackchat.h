#ifndef STACKCHAT_H
#define STACKCHAT_H

#include "Common/basecontroller.h"
#include <QEvent>
#include <Component/Label.h>
#include <QStackedWidget>
#include <Stack/Chat/chat.h>

struct Gdi{
    QString key = "";
    QString txt = "";
    QString ico = "";
};

class StackChat : public BaseController
{
    Q_OBJECT
public:
    explicit StackChat(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *) override;
    bool eventFilter(QObject*,QEvent*) override;

    void initLayout(); //初始化布局
    void initStackWidgets();
    void initGDi(); //初始化全局TAB
    void renderGdi();
    void SelectedGdi(QString);

    void HoverEnter(QObject*);
    void HoverLeave(QObject*);

private:
    QWidget* side; //侧边栏
    QStackedWidget* StackSide;
    QWidget* main; //主面板
    QStackedWidget* StackMain;
    QLabel* start_tip;

    //StackWidgets
    Chat* chat;   //聊天主窗体

    QWidget* tab;
    QMap<int,Gdi*>Gdis;
    QString selected_tab = "";

};

#endif // STACKCHAT_H




