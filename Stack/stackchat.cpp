#pragma execution_character_set("utf-8")
#include "stackchat.h"

#include <Lib/HttpClient.h>

#include <QJsonDocument>
#include <QJsonObject>

#define WIDTH 289

StackChat::StackChat(QWidget *parent) : BaseController(parent)
{
    initSide();
    renderSide();
    SelectedTab("SIDE_TAB_CHAT");

    initMain();
    renderMain();

    //因为文件助手为系统内置的用户，很特殊，所以单独初始化一下
    init_file_helper();

    socket = Socket::Instance()->handle();
    connect(socket, &QTcpSocket::readyRead, this, &StackChat::new_message);
}

void StackChat::init_file_helper()
{
    USER* SYS = new USER();
    SYS->name = "文件助手";
    SYS->job_number = "SYS";
    SYS->avatar = ":/Resources/ico.ico";
    SYS->depid = "0";
    SYS->depname = "技术部";
    SYS->groupid = "0";
    SYS->groupname = "软件研发组";
    SYS->title = "文件小助手";
    SYS->uid = "0";

    chat->append(SYS);
}

void StackChat::new_message()
{
    QString buffer = Socket::Instance()->handle()->readAll();
    if("DQN" != buffer.mid(0,3))
    {
        return;
    }

    buffer = buffer.mid(3);
    int splitIndex = buffer.indexOf("#");

    QString HEADER = buffer.mid(0,splitIndex);
    QString MSG_BODY = buffer.mid(splitIndex+1);

    QStringList HEADERs = HEADER.split("|");
    QString MSG_TYPE = "";
    QString MSG_FROM = "";
    QString MSG_TO = "";
    for(QString hi : HEADERs)
    {
        QStringList his = hi.split(":");
        if("MSGTYPE" == his[0])
        {
            MSG_TYPE = his[1];
        }
        if("FROM" == his[0])
        {
            MSG_FROM = his[1].toUpper();
        }
        if("TO" == his[0])
        {
            MSG_TO = his[1].toUpper();
        }
    }

    int ms = get_time();

    //消息入库
    QString sql = "INSERT INTO `MSG` VALUES (NULL, '"+MSG_TYPE+"', '"+MSG_FROM+"', '"+MSG_TO+"', '"+MSG_BODY+"', "+QString::number(ms)+", 0)";
    Db::Instance()->query(sql);

    //已经聊天过
    if(chat->in_list(MSG_FROM))
    {
        chat->new_msg(MSG_FROM,MSG_TYPE,MSG_BODY);
    }
    else
    {
        //新的聊天
        QString timestamp = QString::number(get_time());

        QString token = md5("DUQINGNIAN"+MSG_FROM+timestamp+"10985");
        token = md5(token+timestamp+"10985");

        HttpClient(path("/client/user/get_detail_by_jobnumber")).success([=](const QString &response) {
            QJsonParseError err_rpt;
            QJsonDocument  jsonDoc = QJsonDocument::fromJson(response.toUtf8(), &err_rpt);
            if(err_rpt.error == QJsonParseError::NoError)
            {
                QJsonObject rootObj = jsonDoc.object();
                if(rootObj.contains("code"))
                {
                    QJsonValue _code = rootObj.value("code");
                    int code = _code.toInt();
                    if(0 == code)
                    {
                        QJsonObject userObj = rootObj.value("user").toObject();

                        USER* EMP = new USER();
                        EMP->name =userObj.value("name").toString();
                        EMP->job_number = userObj.value("job_number").toString().toUpper();
                        EMP->avatar = userObj.value("avatar").toString();
                        EMP->depid = userObj.value("depid").toString();
                        EMP->depname = userObj.value("depname").toString();
                        EMP->groupid = userObj.value("groupid").toString();
                        EMP->groupname = userObj.value("groupname").toString();
                        EMP->title = userObj.value("title").toString();
                        EMP->uid = userObj.value("uid").toString();

                        chat->insert(EMP);
                        chat->new_msg(MSG_FROM,MSG_TYPE,MSG_BODY);

                        QString sql = "INSERT INTO `FRIEND` (`id`, `idx`, `uid`, `job_number`, `name`, `avatar`, `last_msg`, `last_time`, `depid`, `depname`, `groupid`, `groupname`, `title`) VALUES (NULL, '0', '', '"+EMP->job_number+"', '"+EMP->name+"', '"+EMP->avatar+"', '"+MSG_BODY+"', '"+timestamp+"', '"+EMP->depid+"', '"+EMP->depname+"', '"+EMP->groupid+"', '"+EMP->depname+"', '"+EMP->title+"')";
                        qDebug() << sql;
                        Db::Instance()->query(sql);
                    }
                }
            }
        }).param("token", token).param("timestamp", timestamp).param("job_number", MSG_FROM).header("content-type", "application/x-www-form-urlencoded").post();
    }

    if(chatPannelList.count(MSG_FROM) > 0)
    {
        chatPannelList[MSG_FROM]->new_message(MSG_TYPE,MSG_BODY);
    }
}

//左侧点击了好友
void StackChat::friend_targeted(SELECT_UNIT* unit)
{
    selected_unit = unit;
    if(chatPannelList.count(selected_unit->job_number) == 0)
    {
        StackPannel* stack_chat_pannel = new StackPannel(main,selected_unit,true);
        connect(stack_chat_pannel,&StackPannel::post_msg,chat,&Chat::post_msg);
        chatPannelList[selected_unit->job_number] = stack_chat_pannel;
        StackMain->addWidget(chatPannelList[selected_unit->job_number]);
    }
    StackMain->setCurrentWidget(chatPannelList[selected_unit->job_number]);
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

    if("SIDE_TAB_SETTING" == selected_tab)
    {
        //显示设置
    }
    else
    {
        StackSide->setCurrentWidget(chat);
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




