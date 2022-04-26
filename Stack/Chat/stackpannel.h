#ifndef STACKPANNEL_H
#define STACKPANNEL_H

#include <QWidget>
#include <Common/basecontroller.h>
#include <QResizeEvent>
#include <Component/Label.h>

class StackPannel : public BaseController
{
    Q_OBJECT
public:
    explicit StackPannel(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *) override;

    //初始化顶部面板
    void init_info_pannel();
    void set_target(SELECT_UNIT*);
private:
    QWidget* infoPannel;
    Label* avatar;
    Label* name;
    Label* summary;
    Label* info;

    SELECT_UNIT* target;

};

#endif // STACKPANNEL_H
