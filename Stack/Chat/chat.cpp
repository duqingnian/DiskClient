#pragma execution_character_set("utf-8")
#include "chat.h"
#include "chatfrienditem.h"
#include <Component/Label.h>
#include <Lib/HttpClient.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

#define WIDTH 289

Chat::Chat(QWidget *parent) : BaseController(parent)
{
    this->setStyleSheet("font-family: \"Microsoft Yahei\";");
    initTop();
    initFriendList();

    selected_unit = new SELECT_UNIT();
    selected_unit->index = 0;
    selected_unit->job_number = "";

    //载入聊天记录
    load_friends();
}

void Chat::load_friends()
{
    QString select_sql = "select * from FRIEND order by idx desc";
    QSqlQuery sql_query;
    if(!sql_query.exec(select_sql))
    {
        qDebug()<<sql_query.lastError();
    }
    else
    {
        while(sql_query.next())
        {
            USER* f = new USER();

            f->job_number = sql_query.value("job_number").toString();
            f->name = sql_query.value("name").toString();
            f->avatar = sql_query.value("avatar").toString();
            QString last_msg = sql_query.value("last_msg").toString();
            QString last_time = sql_query.value("last_time").toString();
            f->uid = sql_query.value("uid").toString();
            f->depid = sql_query.value("depid").toString();
            f->depname = sql_query.value("depname").toString();
            f->groupid = sql_query.value("groupid").toString();
            f->groupname = sql_query.value("groupname").toString();
            f->title = sql_query.value("last_time").toString();

            append(f);

            QTimer::singleShot(100, this, [=](){
                ChatFriendItem* fw = findChild<ChatFriendItem*>("friend_"+f->job_number);

                fw->set_last_msg(last_msg);
                fw->set_last_time(last_time);
            });
        }
    }
}

void Chat::initTop()
{
    top = new QWidget(this);
    top->setObjectName("chat_top");
    top->move(0,0);
    top->resize(WIDTH,56);
    top->setStyleSheet("#chat_top{background:#fff}");

    Label* more = new Label(top);
    more->resize(24,24);
    more->move(12,20);
    more->setPixmap(QPixmap::fromImage(QImage(":/Resources/Chat/more.png")));

    Label* add = new Label(top);
    add->resize(24,24);
    add->move(250,20);
    add->setPixmap(QPixmap::fromImage(QImage(":/Resources/Chat/write.png")));

    Label* avatar = new Label(top);
    avatar->setObjectName("avatar");
    avatar->resize(36,36);
    avatar->setScaledContents(true);
    avatar->move((top->width() - avatar->width())/2,(top->height() - avatar->height())/2);
    avatar->setPixmap(GetAvatar());
    //avatar->setPixmap(QPixmap::fromImage(QImage(":/Resources/User/noavatar_small.gif")));
    avatar->setStyleSheet("#avatar{border-radius: 10px;background:#EFEDEB;}");

    search = new QWidget(this);
    search->setObjectName("chat_search");
    search->resize(275,34);
    search->setStyleSheet("#chat_search{border-radius: 17px;background:#F3F5F8}#chat_search:hover{border-radius: 17px;background:#E4E7EF}");
    search->move((top->width() - search->width())/2,top->x()+top->height());
    search->setCursor(Qt::IBeamCursor);

    Label* magnifier = new Label(search);
    magnifier->resize(16,16);
    magnifier->move(12,9);
    magnifier->setPixmap(QPixmap::fromImage(QImage(":/Resources/Chat/magnifier.png")));

    Label* search_txt = new Label(search);
    search_txt->setObjectName("search_txt");
    search_txt->setText("搜索");
    search_txt->move(35,6);
    search_txt->setStyleSheet("#search_txt{font-size:14px;color:#838690;}");
}

void Chat::initFriendList()
{
    QLabel* FriendListTip = new QLabel(this);
    FriendListTip->setObjectName("FriendListTip");
    FriendListTip->setText("好友列表");
    FriendListTip->move(10,top->height() + search->height() + 8);
    FriendListTip->setStyleSheet("#FriendListTip{font-size:14px;color:#838690;}");

    scrollArea = new QScrollArea(this);
    scrollArea->setObjectName("scrollAreaChatList");
    scrollArea->move(0,117);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("#scrollAreaChatList{border:0px;}");

    friend_list = new QListWidget(scrollArea);
    friend_list->setObjectName("friend_list");
    friend_list->move(0,0);

    QString css = "#friend_list{border:0px;background:#FFF;}";
    css += "QListWidget{background-color: rgb(255, 255, 255); color:rgb(51,51,51);border: 0px solid rgb(240, 243, 245);outline:0px;}";
    css += "QListWidget::Item{background-color: rgb(255, 255, 255);}";
    css += "QListWidget::Item:hover{background-color: #F3F5F8; }";
    css += "QListWidget::item:selected{background-color: #fff;color:black;border: 0px solid rgb(240, 243, 245);}";
    css += "QListWidget::item:selected:!active{border: 0px solid rgb(240, 243, 245);background-color: #FFF; color:rgb(51,51,51); }";

    friend_list->setStyleSheet(css);
    friend_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(friend_list,&QListWidget::itemClicked,this,&Chat::touch_friend);

    scrollArea->setWidget(friend_list);
}

void Chat::touch_friend(QListWidgetItem *item)
{
    prev_jobnumber = selected_unit->job_number;

    QString touched_job_number = item->data(Qt::UserRole).toString();
    selected_unit->index = ref.key(touched_job_number);
    selected_unit->job_number = touched_job_number;
    selected_unit->unit = friends[touched_job_number];

    int selected_idx = ref.key(touched_job_number);
    friend_list->setCurrentRow(selected_idx);

    //获取当前的widget
    for (int i=0; i<ref.count(); i++) {
        QString jn = ref[i];
        ChatFriendItem* fw = findChild<ChatFriendItem*>("friend_"+jn);
        if(jn == touched_job_number){
            fw->selected();
        }else{
            fw->unselected();
        }
    }
    emit friend_touched(selected_unit);
}

void Chat::post_msg(QString send_to, QString msg_type, QString msg, QString time)
{
    if("" == time)
    {
        QDateTime dateTime(QDateTime::currentDateTime());
        time = dateTime.toString("hh:mm");
    }
    update_meta(send_to,msg_type,msg, time);
}

//来新消息了
void Chat::new_msg(QString job_number, QString msg_type, QString msg)
{
    QDateTime dateTime(QDateTime::currentDateTime());
    QString time = dateTime.toString("hh:mm");

    //更新最后
    QString sql = "update `friend` set `last_msg` = '"+msg+"', `last_time` = "+QString::number(get_time())+" where `job_number` = '"+job_number+"'";
    Db::Instance()->query(sql);

    update_meta(job_number,msg_type,msg,time);
}

void Chat::update_meta(QString job_number, QString msg_type, QString msg, QString time)
{
    ChatFriendItem* fw = findChild<ChatFriendItem*>("friend_"+job_number);

    fw->set_last_msg(msg);
    fw->set_last_time(time);
}

//添加
void Chat::append(USER* _friend)
{
    if(0 == friends.count(_friend->job_number))
    {
        friends.insert(_friend->job_number,_friend);
        ref.insert(ref.count(),_friend->job_number);

        ChatFriendItem* friend_widget = new ChatFriendItem(friend_list->parentWidget());
        friend_widget->setObjectName("friend_"+_friend->job_number);
        friend_widget->set_employee(_friend);
        friend_widget->resize(QSize(WIDTH,65));

        QListWidgetItem* _friend_item = new QListWidgetItem(friend_list);
        QVariant jobnumber(_friend->job_number);
        _friend_item->setData(Qt::UserRole, jobnumber);
        _friend_item->setSizeHint(QSize(WIDTH,65));

        friend_list->setItemWidget(_friend_item, friend_widget);
    }
}

//插入最新聊天
void Chat::insert(USER* _friend)
{
    //如果没有，就添加后插入最新
    if(0 == friends.count(_friend->job_number))
    {
        friends.insert(_friend->job_number,_friend);
        ref.insert(ref.count(),_friend->job_number);

        ChatFriendItem* friend_widget = new ChatFriendItem(friend_list->parentWidget());
        friend_widget->setObjectName("friend_"+_friend->job_number);
        friend_widget->set_employee(_friend);
        friend_widget->resize(QSize(WIDTH,65));

        QListWidgetItem* _friend_item = new QListWidgetItem(friend_list);
        QVariant jobnumber(_friend->job_number);
        _friend_item->setData(Qt::UserRole, jobnumber);
        _friend_item->setSizeHint(QSize(WIDTH,65));

        friend_list->setItemWidget(_friend_item, friend_widget);
    }
    else
    {
        //有的话，就切换到最新一个
    }
}

bool Chat::in_list(QString job_number)
{
    if(0 == friends.count(job_number))
    {
        return false;
    }
    return true;
}


void Chat::resizeEvent(QResizeEvent *)
{
    scrollArea->resize(WIDTH,this->height() - (top->height() + search->height() + 27));
    friend_list->resize(scrollArea->size());
}



