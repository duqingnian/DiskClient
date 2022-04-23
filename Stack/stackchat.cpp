#pragma execution_character_set("utf-8")
#include "stackchat.h"
#include <Component/Label.h>

StackChat::StackChat(QWidget *parent) : BaseController(parent)
{
    initLayout();
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

        QWidget* tab_item = new QWidget(tab);
        tab_item->setObjectName(it.value()->key);
        tab_item->setStyleSheet("#"+it.value()->key+"{background:#fff;}#"+it.value()->key+":hover{background:#f1f1f1;}");
        tab_item->resize(72,51);
        tab_item->move(i*72,1);
        tab_item->setCursor(Qt::PointingHandCursor);
        tab_item->setMouseTracking(true);

        Label* ico = new Label(tab_item);
        ico->setObjectName(it.value()->key+"_ico");
        ico->setStyleSheet("background:transparent;");
        QImage img(":/Resources/Chat/Gdi/"+it.value()->ico+"_0.png");
        ico->setPixmap(QPixmap::fromImage(img));
        ico->setScaledContents(true);
        ico->resize(24,24);
        ico->move(24,4);
        ico->show();

        Label* name = new Label(tab_item);
        name->setObjectName(it.value()->key+"_name");
        name->setText(it.value()->txt);
        name->resize(72,15);
        name->move(0,32);
        name->setAlignment(Qt::AlignCenter);
        name->setStyleSheet("#"+it.value()->key+"_name{color:#8E94A2;font-size:12px;font-weight:400;margin-left:0px;font-family:Microsoft YaHei;}");

        i++;
    }
}


void StackChat::resizeEvent(QResizeEvent *)
{
    side->resize(289,this->height());
    tab->move(0,side->height() - tab->height());
}
