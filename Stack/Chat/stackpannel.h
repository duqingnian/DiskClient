#ifndef STACKPANNEL_H
#define STACKPANNEL_H

#include <QWidget>
#include <Common/basecontroller.h>
#include <QResizeEvent>
#include <Component/Label.h>
#include <QTextEdit>

class StackPannel : public BaseController
{
    Q_OBJECT
public:
    explicit StackPannel(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *) override;

    //初始化顶部面板
    void init_info_pannel();
    void init_action_pannel();
    void set_target(SELECT_UNIT*);

private:
    QWidget* infoPannel;
    Label* avatar;
    Label* name;
    Label* summary;
    Label* info;
    QWidget* actionPannel;
    QWidget* chatList;
    SELECT_UNIT* target;
    Label* action_send;
    Label* action_add;
    QTextEdit* input;
private slots:
    void resize_input();
    void send_msg();
};

#endif // STACKPANNEL_H
