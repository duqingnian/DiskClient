#pragma execution_character_set("utf-8")
#include "stackchat.h"

StackChat::StackChat(QWidget *parent) : BaseController(parent)
{
    initLayout();
    initStackWidgets();
    SelectedGdi("GDI_CHAT");
}


void StackChat::initLayout()
{
    side = new QWidget(this);
    side->setObjectName("LAYOUT_SIDE");
    side->setStyleSheet("#LAYOUT_SIDE{background-color:#fff;border-right:1px solid #ECEEF3;}");

    tab = new QWidget(side);
    tab->setObjectName("GLOBAL_TAB");
    tab->resize(288,52);
    tab->setStyleSheet("#GLOBAL_TAB{border-top:1px solid #ECEEF3;}");
    tab->move(0,side->height() - tab->height());

    initGDi();
}

void StackChat::initStackWidgets()
{
    StackSide = new QStackedWidget(side);
    StackSide->setObjectName("StackSide");
    StackSide->move(0,0);

    chat = new Chat(side);
    chat->setObjectName("StackChat");

    StackSide->addWidget(chat);

    StackSide->setCurrentWidget(chat);
}

//初始化tab
void StackChat::initGDi()
{
    Gdi* dep = new Gdi();
    dep->key = "GDI_DEP";
    dep->txt = "部门";
    dep->ico = "dep";

    Gdi* group = new Gdi();
    group->key = "GDI_GROUP";
    group->txt = "群组";
    group->ico = "group";

    Gdi* chat = new Gdi();
    chat->key = "GDI_CHAT";
    chat->txt = "聊天";
    chat->ico = "chat";

    Gdi* setting = new Gdi();
    setting->key = "GDI_SETTING";
    setting->txt = "设置";
    setting->ico = "setting";

    Gdis[0] = dep;
    Gdis[1] = group;
    Gdis[2] = chat;
    Gdis[3] = setting;

    renderGdi();
}

//渲染tab
void StackChat::renderGdi()
{
    int  i = 0;
    QMapIterator<int,Gdi*> it(Gdis);
    while(it.hasNext())
    {
        it.next();

        Label* tab_item = new Label(tab);
        tab_item->setObjectName(it.value()->key);
        tab_item->setStyleSheet("#"+it.value()->key+"{background:#fff;}");
        tab_item->resize(72,51);
        tab_item->move(i*72,1);
        tab_item->setCursor(Qt::PointingHandCursor);
        tab_item->setAttribute(Qt::WA_Hover,true);
        tab_item->installEventFilter(this);
        connect(tab_item,&Label::clicked,this,[=](){SelectedGdi(it.value()->key);});

        Label* ico = new Label(tab_item);
        ico->setObjectName(it.value()->key+"_ico");
        ico->setStyleSheet("background:transparent;");
        ico->setPixmap(QPixmap::fromImage(QImage(":/Resources/Chat/Gdi/"+it.value()->ico+"_0.png")));
        ico->setScaledContents(true);
        ico->resize(24,24);
        ico->move(24,4);
        connect(ico,&Label::clicked,this,[=](){SelectedGdi(it.value()->key);});

        Label* name = new Label(tab_item);
        name->setObjectName(it.value()->key+"_name");
        name->setText(it.value()->txt);
        name->resize(72,15);
        name->move(0,32);
        name->setAlignment(Qt::AlignCenter);
        name->setStyleSheet("#"+it.value()->key+"_name{color:#8E94A2;font-size:12px;font-weight:400;margin-left:0px;font-family:Microsoft YaHei;}");
        connect(name,&Label::clicked,this,[=](){SelectedGdi(it.value()->key);});

        i++;
    }
}

void StackChat::resizeEvent(QResizeEvent *)
{
    side->resize(289,this->height());
    tab->move(0,side->height() - tab->height());

    StackSide->resize(288,this->height() - 52);
}

bool StackChat::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::HoverEnter)
    {
        HoverEnter(obj);
        return true;
    }
    else if (event->type() == QEvent::HoverLeave)
    {
        HoverLeave(obj);
        return true;
    }

    return QWidget::eventFilter(obj, event);
}

void StackChat::HoverEnter(QObject* obj)
{
    //边栏tab
    if("GDI_" == obj->objectName().mid(0,4))
    {
        if(selected_tab != obj->objectName())
        {
            Label* thumb = findChild<Label*>(obj->objectName()+"_ico");
            thumb->setPixmap(QPixmap::fromImage(QImage(":/Resources/Chat/Gdi/"+obj->objectName().mid(4).toLower()+"_1.png")));
        }
    }
}


void StackChat::HoverLeave(QObject* obj)
{
    //边栏tab
    if("GDI_" == obj->objectName().mid(0,4))
    {
        if(selected_tab != obj->objectName())
        {
            Label* thumb = findChild<Label*>(obj->objectName()+"_ico");
            thumb->setPixmap(QPixmap::fromImage(QImage(":/Resources/Chat/Gdi/"+obj->objectName().mid(4).toLower()+"_0.png")));
        }
    }
}

void StackChat::SelectedGdi(QString key)
{
    selected_tab = key;

    QMapIterator<int,Gdi*> it(Gdis);
    while(it.hasNext())
    {
        it.next();
        QString objname = it.value()->key;

        Label* ico = findChild<Label*>(objname+"_ico");
        Label* name = findChild<Label*>(objname+"_name");
        if(selected_tab == objname)
        {
            ico->setPixmap(QPixmap::fromImage(QImage(":/Resources/Chat/Gdi/"+objname.mid(4).toLower()+"_2.png")));
            name->setStyleSheet("#"+it.value()->key+"_name{color:#21D86A;font-size:12px;font-weight:400;margin-left:0px;font-family:Microsoft YaHei;}");
        }
        else
        {
            ico->setPixmap(QPixmap::fromImage(QImage(":/Resources/Chat/Gdi/"+objname.mid(4).toLower()+"_0.png")));
            name->setStyleSheet("#"+it.value()->key+"_name{color:#8E94A2;font-size:12px;font-weight:400;margin-left:0px;font-family:Microsoft YaHei;}");
        }
    }
}







