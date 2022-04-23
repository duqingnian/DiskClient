#ifndef STACKCHAT_H
#define STACKCHAT_H

#include "Common/basecontroller.h"

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

    void initLayout(); //初始化布局
    void initGDi(); //初始化全局TAB

    void renderGdi();
private:
    QWidget* side; //侧边栏
    QWidget* main; //主面板

    QWidget* tab;
    QMap<int,Gdi*>Gdis;
    QString selected_tab;
};

#endif // STACKCHAT_H
