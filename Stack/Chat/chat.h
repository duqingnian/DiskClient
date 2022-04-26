#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <Common/basecontroller.h>
#include <QLabel>
#include <SocketData/FriendUnit.h>
#include <QResizeEvent>
#include <QScrollArea>

struct SELECT_UNIT{
    int index = 0;
    QString job_number = "";
    FRIEND_UNIT* unit;
};

class Chat : public BaseController
{
    Q_OBJECT
public:
    explicit Chat(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *) override;
    void initTop();
    void initFriendList();

    //渲染好友列表
    void render_friends();

    //点击选择了好友
    void touch_friend();
signals:

private:
    SELECT_UNIT* selected_unit;
    QString prev_jobnumber = "";

    QMap<int,QString> ref;
    QMap<QString, FRIEND_UNIT*> friends;

    QWidget* top;
    QWidget* friend_list;
    QWidget* search;

    QScrollArea* scrollArea;
private slots:

};

#endif // CHAT_H
