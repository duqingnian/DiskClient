#pragma execution_character_set("utf-8")
#include "chat.h"
#include <Component/Label.h>

#define WIDTH 289

Chat::Chat(QWidget *parent) : BaseController(parent)
{
    this->setStyleSheet("font-family: \"Microsoft Yahei\";");
    initTop();
    initFriendList();

    for (int i=0; i<45; i++) {
        FRIEND_UNIT* AI = new FRIEND_UNIT();
        AI->name = "AI机器人-"+QString::number(i);
        AI->job_number = "SYS"+QString::number(i);
        AI->avatar = "";

        friends.insert("SYS"+QString::number(i),AI);
        ref.insert(i,"SYS"+QString::number(i));
    }

    selected_unit = new SELECT_UNIT();
    selected_unit->index = 0;
    selected_unit->job_number = "";

    render_friends();
}

void Chat::resizeEvent(QResizeEvent *)
{
    scrollArea->resize(WIDTH,this->height() - (top->height() + search->height()));
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
    add->setPixmap(QPixmap::fromImage(QImage(":/Resources/Chat/add.png")));

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
    scrollArea = new QScrollArea(this);
    scrollArea->move(0,top->height() + search->height());
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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
        FRIEND_UNIT* funit = friends[job_number];

        Label* _friend = new Label(friend_list);
        _friend->setObjectName("funit_"+funit->job_number);
        _friend->resize(WIDTH,65);
        _friend->setStyleSheet("#funit_"+funit->job_number+"{background:#fff;border-bottom:1px solid #F3F5F8;}#funit_"+funit->job_number+":hover{background:#F3F5F8;}");
        _friend->move(0,i*65);
        _friend->setCursor(Qt::PointingHandCursor);

        Label* avatar = new Label(_friend);
        avatar->setObjectName("funit_avatar_"+funit->job_number);
        avatar->resize(40,40);
        avatar->setScaledContents(true);
        avatar->move(12,(_friend->height() - avatar->height())/2);
        QPixmap map = QPixmap::fromImage(QImage(":/Resources/User/noavatar_middle.gif"));
        avatar->setPixmap(map);
        avatar->setStyleSheet("#funit_avatar_"+funit->job_number+"{border-radius: 18px;background:red;}");

        Label* name = new Label(_friend);
        name->setObjectName("funit_name_"+funit->job_number);\
        name->setStyleSheet("#funit_name_"+funit->job_number+"{font-size:14px;color:#000;}");
        name->setText(funit->name);
        name->move(62,avatar->y());

        connect(_friend,&Label::clicked,this,[=](){

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
        Label* _prev_selected = findChild<Label*>("funit_"+prev_jobnumber);
        Label* _prev_selected_name = findChild<Label*>("funit_name_"+prev_jobnumber);
        _prev_selected->setStyleSheet("#funit_"+prev_jobnumber+"{background:#fff;border-bottom:1px solid #F3F5F8;}#funit_"+prev_jobnumber+":hover{background:#F3F5F8;}");
        _prev_selected_name->setStyleSheet("#funit_name_"+prev_jobnumber+"{color:#000;font-size:14px;}");
    }

    //设置新的选中的
    if("" != selected_unit->job_number)
    {
        Label* _prev_selected = findChild<Label*>("funit_"+selected_unit->job_number);
        Label* _prev_selected_name = findChild<Label*>("funit_name_"+selected_unit->job_number);
        _prev_selected->setStyleSheet("#funit_"+selected_unit->job_number+"{background:#1DCF67;color:#fff;border-bottom:1px solid #F3F5F8;}#funit_"+selected_unit->job_number+":hover{background:#1DCF67;}");
        _prev_selected_name->setStyleSheet("#funit_name_"+selected_unit->job_number+"{color:#fff;font-size:14px;}");
    }

}


























