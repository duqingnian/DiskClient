#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <Common/basecontroller.h>
#include <QLabel>
#include <QListWidget>
#include <QResizeEvent>
#include <QScrollArea>

class Chat : public BaseController
{
    Q_OBJECT
public:
    explicit Chat(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *) override;
    void initTop();
    void initFriendList();

    //载入聊天记录
    void load_friends();

    //来了新消息
    void new_msg(QString job_number,QString msg_type,QString msg);

    //追加一个聊天
    void append(USER*);

    //插入一个聊天
    void insert(USER*);

    //是不是有这个聊天
    bool in_list(QString job_number);

    //更新最后的消息和时间
    void update_meta(QString job_number,QString msg_type,QString msg, QString time);
signals:
    void friend_touched(SELECT_UNIT*);
private:
    SELECT_UNIT* selected_unit;
    QString prev_jobnumber = "";

    QMap<int,QString> ref;
    QMap<QString, USER*> friends;

    QWidget* top;
    QListWidget* friend_list;
    QWidget* search;

    QScrollArea* scrollArea;
public slots:
    //点击选择了好友
    void touch_friend(QListWidgetItem *item);
    //发送消息
    void post_msg(QString send_to,QString msg_type,QString msg, QString time);
};

#endif // CHAT_H
