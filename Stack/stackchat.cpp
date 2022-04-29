#pragma execution_character_set("utf-8")
#include "stackchat.h"

#define WIDTH 289

StackChat::StackChat(QWidget *parent) : BaseController(parent)
{
    initSide();
    renderSide();
    SelectedTab("SIDE_TAB_CHAT");

    initMain();
    renderMain();

    socket = Socket::Instance()->handle();
    connect(socket, &QTcpSocket::readyRead, this, &StackChat::new_message);
}

void StackChat::new_message()
{
    //收到消息，解析出job_number，侧边栏消息数量+1 右侧消息面板+1
    QByteArray buffer;

    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    socketStream.startTransaction();
    socketStream >> buffer;

    if(!socketStream.commitTransaction())
    {
        QString message = QString("%1 :: Waiting for more data to come..").arg(socket->socketDescriptor());
        qDebug() << message;
        return;
    }

    if(buffer.length() > 128)
    {
        QString header = buffer.mid(0,128);
        QStringList headers = header.split("|");
        QString msg_type = "";
        QString msg_from = "";
        QString msg_to = "";
        for(QString hi : headers)
        {
            QStringList his = hi.split(":");
            if("MSGTYPE" == his[0])
            {
                msg_type = his[1];
            }
            if("FROM" == his[0])
            {
                msg_from = his[1];
            }
            if("TO" == his[0])
            {
                msg_to = his[1];
            }
        }
        qDebug() << "MSGTYPE=" << msg_type << "FROM=" << msg_from << "TO=" << msg_to;
        QString MSG_BODY = buffer.mid(128);
        qDebug() << "MSG_BODY=" << MSG_BODY;

        if(chatPannelList.count(selected_unit->job_number) > 0)
        {
            chatPannelList[msg_from]->new_message(msg_type,MSG_BODY);
        }
    }
}

void StackChat::initSide()
{
    side = new QWidget(this);
    side->setObjectName("LAYOUT_SIDE");
    side->setStyleSheet("#LAYOUT_SIDE{background-color:#fff;border-right:1px solid #ECEEF3;}");

    tab = new QWidget(side);
    tab->setObjectName("GLOBAL_TAB");
    tab->resize(WIDTH - 1,52);
    tab->setStyleSheet("#GLOBAL_TAB{border-top:1px solid #ECEEF3;}");
    tab->move(0,side->height() - tab->height());

    SIDE_TAB* tab_dep = new SIDE_TAB();
    tab_dep->key = "SIDE_TAB_DEP";
    tab_dep->txt = "部门";
    tab_dep->ico = "dep";

    SIDE_TAB* tab_group = new SIDE_TAB();
    tab_group->key = "SIDE_TAB_GROUP";
    tab_group->txt = "群组";
    tab_group->ico = "group";

    SIDE_TAB* tab_chat = new SIDE_TAB();
    tab_chat->key = "SIDE_TAB_CHAT";
    tab_chat->txt = "聊天";
    tab_chat->ico = "chat";

    SIDE_TAB* tab_setting = new SIDE_TAB();
    tab_setting->key = "SIDE_TAB_SETTING";
    tab_setting->txt = "设置";
    tab_setting->ico = "setting";

    side_tabs[0] = tab_dep;
    side_tabs[1] = tab_group;
    side_tabs[2] = tab_chat;
    side_tabs[3] = tab_setting;

    //side stack
    StackSide = new QStackedWidget(side);
    StackSide->setObjectName("StackSide");
    StackSide->move(0,0);

    chat = new Chat(side);
    chat->setObjectName("StackChat");
    connect(chat,&Chat::friend_touched,this,[=](SELECT_UNIT* unit){
        friend_targeted(unit);
    });

    StackSide->addWidget(chat);

    StackSide->setCurrentWidget(chat);
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

//渲染界面
void StackChat::renderSide()
{
    //渲染侧边栏tab
    int  i = 0;
    QMapIterator<int,SIDE_TAB*> it(side_tabs);
    while(it.hasNext())
    {
        it.next();

        Label* tab_item = new Label(tab);
        tab_item->setObjectName(it.value()->key);
        //tab_item->setStyleSheet("#"+it.value()->key+"{background:red;}");
        tab_item->resize(72,51);
        tab_item->move(i*72,1);
        tab_item->setCursor(Qt::PointingHandCursor);
        tab_item->setAttribute(Qt::WA_Hover,true);
        tab_item->installEventFilter(this);
        connect(tab_item,&Label::clicked,this,[=](){SelectedTab(it.value()->key);});

        Label* ico = new Label(tab_item);
        ico->setObjectName(it.value()->key+"_ico");
        ico->setScaledContents(true);
        ico->resize(24,24);
        ico->move(24,4);
        ico->raise();
        connect(ico,&Label::clicked,this,[=](){SelectedTab(it.value()->key);});

        Label* name = new Label(tab_item);
        name->setObjectName(it.value()->key+"_name");
        name->setText(it.value()->txt);
        name->resize(72,15);
        name->move(0,32);
        name->setAlignment(Qt::AlignCenter);
        name->setStyleSheet("#"+it.value()->key+"_name{color:#8E94A2;font-size:12px;font-weight:400;margin-left:0px;font-family:Microsoft YaHei;}");
        connect(name,&Label::clicked,this,[=](){SelectedTab(it.value()->key);});

        i++;
    }
}

//选中tab
void StackChat::SelectedTab(QString key)
{
    selected_tab = key;

    QMapIterator<int,SIDE_TAB*> it(side_tabs);
    while(it.hasNext())
    {
        it.next();
        QString objname = it.value()->key;
        Label* ico = findChild<Label*>(objname+"_ico");
        Label* name = findChild<Label*>(objname+"_name");
        if(selected_tab == objname)
        {
            ico->setPixmap(QPixmap::fromImage(QImage(":/Resources/Chat/Gdi/"+objname.mid(9).toLower()+"_2.png")));
            name->setStyleSheet("#"+it.value()->key+"_name{color:#21D86A;font-size:12px;font-weight:400;margin-left:0px;font-family:Microsoft YaHei;}");
        }
        else
        {
            ico->setPixmap(QPixmap::fromImage(QImage(":/Resources/Chat/Gdi/"+objname.mid(9).toLower()+"_0.png")));
            name->setStyleSheet("#"+it.value()->key+"_name{color:#8E94A2;font-size:12px;font-weight:400;margin-left:0px;font-family:Microsoft YaHei;}");
        }
    }
}

void StackChat::HoverEnter(QObject* obj)
{
    //边栏tab
    if("SIDE_TAB_" == obj->objectName().mid(0,9))
    {
        if(selected_tab != obj->objectName())
        {
            Label* thumb = findChild<Label*>(obj->objectName()+"_ico");
            thumb->setPixmap(QPixmap::fromImage(QImage(":/Resources/Chat/Gdi/"+obj->objectName().mid(9).toLower()+"_1.png")));
        }
    }
}


void StackChat::HoverLeave(QObject* obj)
{
    //边栏tab
    if("SIDE_TAB_" == obj->objectName().mid(0,9))
    {
        if(selected_tab != obj->objectName())
        {
            Label* thumb = findChild<Label*>(obj->objectName()+"_ico");
            thumb->setPixmap(QPixmap::fromImage(QImage(":/Resources/Chat/Gdi/"+obj->objectName().mid(9).toLower()+"_0.png")));
        }
    }
}

//初始化内容区域
void StackChat::initMain()
{
    main = new QWidget(this);
    main->setObjectName("GLOBAL_MAIN");

    StackMain = new QStackedWidget(main);
    StackMain->setObjectName("stack_main");
    StackMain->move(0,0);

    stack_empty = new StackEmpty(main);
    StackMain->addWidget(stack_empty);
    StackMain->setCurrentWidget(stack_empty);
}

//渲染内容区域
void StackChat::renderMain()
{

}

//左侧点击了好友
void StackChat::friend_targeted(SELECT_UNIT* unit)
{
    selected_unit = unit;
    if(chatPannelList.count(selected_unit->job_number) == 0)
    {
        StackPannel* stack_chat_pannel = new StackPannel(main,selected_unit);
        chatPannelList[selected_unit->job_number] = stack_chat_pannel;
        StackMain->addWidget(chatPannelList[selected_unit->job_number]);
    }
    StackMain->setCurrentWidget(chatPannelList[selected_unit->job_number]);
}

void StackChat::resizeEvent(QResizeEvent *)
{
    side->resize(WIDTH,this->height());
    StackSide->resize(WIDTH - 1,this->height() - 52);
    tab->move(0,side->height() - tab->height());

    main->resize(this->width() - WIDTH,this->height());
    main->move(WIDTH,0);
    StackMain->resize(main->size());
    //StackMain->currentWidget()->resize(main->size());
}




