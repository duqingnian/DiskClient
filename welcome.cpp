#pragma execution_character_set("utf-8")
#include "welcome.h"
#include "ui_welcome.h"
#include <QDebug>
#include <QScreen>

Welcome::Welcome(QWidget *parent) : BaseWindow(parent) , ui(new Ui::Welcome)
{
    ui->setupUi(this);

    InitLayout();

    //uid= "ADOSTR445beeL4aAU5oWzIinw6S9SWp8RidNy7eJNb7HSpgkbApGwoAyYSRwZYCHeP=E=" ,  "杜庆年" , "营销中心" , "技术支持" , "程序员" , "CZML669"
    //qDebug() << "uid=" << user->uid << ", " <<user->name << "," << user->depname << "," << user->groupname<< "," << user->title << "," << user->job_number;

    //初始化Sqlite数据库
}

void Welcome::set_auth(Auth* _auth)
{
    auth = _auth;
}

//初始化Layout
void Welcome::InitLayout()
{
    //整体的大小，宽度70%，最小1024， 高度86%，最小768
    screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    _width = screenGeometry.width() * 0.72;
    _height = screenGeometry.height() * 0.86;
    if(_width < 1024)
    {
        _width = 1024;
    }
    if(_height < 768)
    {
        _height = 768;
    }
    this->resize(_width,_height);
    //标题栏
    this->m_TitleBar->setButtonType(MIN_MAX_BUTTON);
    this->m_TitleBar->set_width(_width);
    this->m_TitleBar->set_bg_color(43,87,154);
    this->m_TitleBar->show_windows();
    this->m_TitleBar->setCenterTitle("常州美莱共享文件管理系统-"+user->name+"-"+user->depname,"#FFFFFF");
    //TAB
    TAB = new QWidget(this);
    TAB->setObjectName("GLOBAL_TAB");
    //TAB->resize(this->width()-this->border_width*2,32);
    TAB->move(this->border_width,this->m_TitleBar->y()+this->m_TitleBar->height());
    TAB->setStyleSheet("#GLOBAL_TAB{background-color:#2B579A;}");

    StackWidget = new QStackedWidget(this);
    StackWidget->setObjectName("layout_stack_widget");
    StackWidget->resize(_width-this->border_width*2,_height - this->border_width*2 - 67);
    StackWidget->move(this->border_width,67+this->border_width);
    StackWidget->setStyleSheet("#layout_stack_widget{background:#F9F9F9;}");

    stack_file = new StackFileMain();
    stack_chat = new StackChat();
    stack_help = new StackHelp();

    StackWidget->addWidget(stack_file);
    StackWidget->addWidget(stack_chat);
    StackWidget->addWidget(stack_help);

    StackWidget->setCurrentWidget(stack_file);

    render_tab();
}

void Welcome::render_tab()
{
    TABMETA* tab_chat = new TABMETA();
    tab_chat->objname = "tab_chat";
    tab_chat->txt = "云信";
    tab_chat->bgcolor = "#ffffff";
    left_tab_metas[tab_chat->objname] = tab_chat;


    TABMETA* tab_disk = new TABMETA();
    tab_disk->objname = "tab_disk";
    tab_disk->txt = "云盘";
    tab_disk->bgcolor = "#F3F3F3";
    left_tab_metas[tab_disk->objname] = tab_disk;

    TABMETA* tab_help = new TABMETA();
    tab_help->objname = "tab_help";
    tab_help->txt = "帮助信息";
    tab_help->bgcolor = "#ffff00";
    right_tab_metas[tab_help->objname] = tab_help;

    int x = 0;
    QMapIterator<QString,TABMETA*> it(left_tab_metas);
    while(it.hasNext()){
        it.next();
        Label* tab_item = new Label(TAB);
        tab_item->setObjectName(it.value()->objname);
        tab_item->resize(it.value()->width,32);
        tab_item->setText(it.value()->txt);
        tab_item->setAlignment(Qt::AlignCenter);
        tab_item->move(x,0);
        x += tab_item->width();
        tab_item->setStyleSheet("#"+tab_item->objectName()+"{font-size:12px;font-family: \"Microsoft Yahei\";color:#fff;}#"+tab_item->objectName()+":hover{font-size:12px;font-family: \"Microsoft Yahei\";color:#fff;background:#124078;}");

        connect(tab_item, &Label::clicked, this, [=](){ this->SelectTab(tab_item->objectName()); });
        TabItems.append(tab_item);
    }
    SelectTab("tab_disk");

    TAB_ATTR = new QWidget(TAB);
    TAB_ATTR->setObjectName("TAB_ATTR");
}

void Welcome::render_tab_attr()
{
    int x = 0;
    QMapIterator<QString,TABMETA*> it(right_tab_metas);
    while(it.hasNext()) {
        it.next();
        int width = it.value()->txt.length() * 15 + 35;
        Label* tab_item = new Label(TAB_ATTR);
        tab_item->setObjectName(it.value()->objname);
        tab_item->setText(it.value()->txt);
        tab_item->setAlignment(Qt::AlignCenter);
        tab_item->resize(width,32);
        tab_item->move(x,0);
        x += width;

        tab_item->setStyleSheet("#"+tab_item->objectName()+"{font-size:12px;font-family: \"Microsoft Yahei\";color:#fff;}#"+tab_item->objectName()+":hover{font-size:12px;font-family: \"Microsoft Yahei\";color:#fff;background:#124078;}");

        connect(tab_item, &Label::clicked, this, [=](){ this->SelectTab(tab_item->objectName()); });
        TabItems.append(tab_item);
    }
}

void Welcome::SelectTab(QString tab_key)
{
    for (int i=0;i<TabItems.length(); i++)
    {
        if(TabItems[i]->objectName() == tab_key)
        {
            ActiveTAB = tab_key;
            QString bgcolor = "#FFF";
            if(left_tab_metas.contains(tab_key))
            {
                bgcolor = left_tab_metas[tab_key]->bgcolor;
            }
            else if(right_tab_metas.contains(tab_key))
            {
                bgcolor = right_tab_metas[tab_key]->bgcolor;
            }
            TabItems[i]->setStyleSheet("#"+TabItems[i]->objectName()+"{font-size:12px;font-weight:bold;font-family: \"Microsoft Yahei\";color:#2B579A;background:"+bgcolor+";}");
        }
        else
        {
            TabItems[i]->setStyleSheet("#"+TabItems[i]->objectName()+"{font-size:12px;font-family: \"Microsoft Yahei\";color:#fff;}#"+TabItems[i]->objectName()+":hover{font-size:12px;font-family: \"Microsoft Yahei\";color:#fff;background:#124078;}");
        }
    }

    if("tab_chat" == ActiveTAB)
    {
        StackWidget->setCurrentWidget(stack_chat);
    }
    else if("tab_disk" == ActiveTAB)
    {
        StackWidget->setCurrentWidget(stack_file);
    }
    else if("tab_help" == ActiveTAB)
    {
        StackWidget->setCurrentWidget(stack_help);
    }
    else
    {}
}


void Welcome::resizeEvent(QResizeEvent *e)
{
    int width = this->width()-this->border_width*2;
    this->m_TitleBar->center_title->move( (width - this->m_TitleBar->center_title->width())/2, 4);
    TAB->resize(width,32);
    TAB_ATTR->move(TAB->width() - TAB_ATTR->width(),0);
    render_tab_attr();
    StackWidget->resize(width,this->height() - this->border_width*2 - 67);

    return QWidget::resizeEvent(e);
}



Welcome::~Welcome()
{
    delete ui;
}












