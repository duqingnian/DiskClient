#pragma execution_character_set("utf-8")
#include "chat.h"
#include <Component/Label.h>

#define WIDTH 289

Chat::Chat(QWidget *parent) : BaseController(parent)
{
    this->setStyleSheet("font-family: \"Microsoft Yahei\";");
    initTop();
    initFriendList();

    for (int i=0; i<5; i++) {
        USER* AI = new USER();
        AI->name = "文件小助手-"+QString::number(i);
        AI->job_number = "TEST"+QString::number(i);
        AI->avatar = "";
        AI->depid = "0";
        AI->depname = "技术部"+QString::number(i);
        AI->groupid = "0";
        AI->groupname = "软件研发组"+QString::number(i);
        AI->title = "文件小助手";
        AI->uid = "0";

        friends.insert("TEST"+QString::number(i),AI);
        ref.insert(i,"TEST"+QString::number(i));
    }

    selected_unit = new SELECT_UNIT();
    selected_unit->index = 0;
    selected_unit->job_number = "";

    render_friends();
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
    QPixmap map = QPixmap::fromImage(QImage(":/Resources/User/noavatar_small.gif"));
    avatar->setPixmap(map);
    avatar->setStyleSheet("#avatar{border-radius: 18px;background:red;}");

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
    //scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("#scrollAreaChatList{border:0px;}");

    friend_list = new QWidget(scrollArea);
    friend_list->setObjectName("friend_list");
    friend_list->move(0,0);
    friend_list->resize(WIDTH,350);
    friend_list->setStyleSheet("#friend_list{background:#fff}");

    scrollArea->setWidget(friend_list);
}

void Chat::render_friends()
{
    friend_list->resize(WIDTH,ref.count() * 65);

    int i = 0;
    QMapIterator<int,QString> it(ref);
    while(it.hasNext())
    {
        it.next();
        QString job_number = it.value();
        USER* funit = friends[job_number];

        Label* _friend = new Label(friend_list);
        _friend->setObjectName("funit_"+funit->job_number);
        _friend->resize(WIDTH,65);
        _friend->setStyleSheet("#funit_"+funit->job_number+"{background:#fff;}#funit_"+funit->job_number+":hover{background:#F3F5F8;}");
        _friend->move(0,i*65);
        _friend->setCursor(Qt::PointingHandCursor);

        Label* avatar = new Label(_friend);
        avatar->setObjectName("funit_avatar_"+funit->job_number);
        avatar->resize(40,40);
        avatar->setScaledContents(true);
        avatar->move(12,(_friend->height() - avatar->height())/2);
        QPixmap map = QPixmap::fromImage(QImage(":/Resources/ico.ico"));
        avatar->setPixmap(map);
        avatar->setStyleSheet("#funit_avatar_"+funit->job_number+"{border-radius: 18px;}");

        Label* name = new Label(_friend);
        name->setObjectName("funit_name_"+funit->job_number);\
        name->setStyleSheet("#funit_name_"+funit->job_number+"{font-size:14px;color:#000;}");
        name->setText(funit->name);
        name->move(62,avatar->y());

        //时间
        Label* last_time = new Label(_friend);
        last_time->setObjectName("last_time_"+funit->job_number);
        last_time->setStyleSheet("#last_time_"+funit->job_number+"{color:#B9B9B9;font-size:12px;}");
        last_time->setMinimumWidth(68);
        last_time->move(WIDTH - 80, name->y());
        last_time->setText("22/04/20");
        last_time->setAlignment(Qt::AlignRight);

        //消息
        QString msg = "我是消息，我是测试消息，测试消息长度最长可以到多少";
        if(msg.length() > 14)
        {
            msg = msg.mid(0,14)+"...";
        }
        Label* last_msg = new Label(_friend);
        last_msg->setObjectName("last_msg_"+funit->job_number);
        last_msg->setMaximumWidth(210);
        last_msg->setStyleSheet("#last_msg_"+funit->job_number+"{color:#999999;font-size:13px;}");
        last_msg->move(name->x(),35 );
        last_msg->setText(msg);

        connect(avatar,&Label::clicked,this,[=](){

            prev_jobnumber = selected_unit->job_number;

            selected_unit->index = it.key();
            selected_unit->job_number = funit->job_number;
            selected_unit->unit = funit;

            touch_friend();
        });
        connect(_friend,&Label::clicked,this,[=](){

            prev_jobnumber = selected_unit->job_number;

            selected_unit->index = it.key();
            selected_unit->job_number = funit->job_number;
            selected_unit->unit = funit;

            touch_friend();
        });
        connect(name,&Label::clicked,this,[=](){

            prev_jobnumber = selected_unit->job_number;

            selected_unit->index = it.key();
            selected_unit->job_number = funit->job_number;
            selected_unit->unit = funit;

            touch_friend();
        });
        connect(last_msg,&Label::clicked,this,[=](){

            prev_jobnumber = selected_unit->job_number;

            selected_unit->index = it.key();
            selected_unit->job_number = funit->job_number;
            selected_unit->unit = funit;

            touch_friend();
        });
        connect(last_time,&Label::clicked,this,[=](){

            prev_jobnumber = selected_unit->job_number;

            selected_unit->index = it.key();
            selected_unit->job_number = funit->job_number;
            selected_unit->unit = funit;

            touch_friend();
        });

        i++;
    }
}

void Chat::touch_friend()
{
    //解除已经选中的
    if("" != prev_jobnumber)
    {
        Label* _prev_selected           = findChild<Label*>("funit_"+prev_jobnumber);
        Label* _prev_selected_name      = findChild<Label*>("funit_name_"+prev_jobnumber);
        Label* _prev_selected_last_time = findChild<Label*>("last_time_"+prev_jobnumber);
        Label* _prev_selected_last_msg  = findChild<Label*>("last_msg_"+prev_jobnumber);

        _prev_selected->setStyleSheet("#funit_"+prev_jobnumber+"{background:#fff;border-bottom:1px solid #F3F5F8;}#funit_"+prev_jobnumber+":hover{background:#F3F5F8;}");
        _prev_selected_name->setStyleSheet("#funit_name_"+prev_jobnumber+"{color:#000;font-size:14px;}");
        _prev_selected_last_time->setStyleSheet("#last_time_"+prev_jobnumber+"{color:#B9B9B9;font-size:12px;}");
        _prev_selected_last_msg->setStyleSheet("#last_msg_"+prev_jobnumber+"{color:#999999;font-size:14px;}");
    }

    //设置新的选中的
    if("" != selected_unit->job_number)
    {
        Label* _curr_selected      = findChild<Label*>("funit_"+selected_unit->job_number);
        Label* _curr_selected_name = findChild<Label*>("funit_name_"+selected_unit->job_number);
        Label* _curr_last_time     = findChild<Label*>("last_time_"+selected_unit->job_number);
        Label* _curr_last_msg      = findChild<Label*>("last_msg_"+selected_unit->job_number);

        _curr_selected->setStyleSheet("#funit_"+selected_unit->job_number+"{background:#1DCF67;color:#fff;border-bottom:1px solid #F3F5F8;}#funit_"+selected_unit->job_number+":hover{background:#1DCF67;}");
        _curr_selected_name->setStyleSheet("#funit_name_"+selected_unit->job_number+"{color:#fff;font-size:14px;}");
        _curr_last_time->setStyleSheet("#last_time_"+selected_unit->job_number+"{color:#fff;font-size:12px;}");
        _curr_last_msg->setStyleSheet("#last_msg_"+selected_unit->job_number+"{color:#fff;font-size:13px;}");

        //改变主界面
        emit friend_touched(selected_unit);
    }

}

void Chat::resizeEvent(QResizeEvent *)
{
    scrollArea->resize(WIDTH,this->height() - (top->height() + search->height() + 27));
}



