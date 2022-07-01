#pragma execution_character_set("utf-8")
#include "stackpannel.h"
#include <QDateTime>
#include <QDebug>
#include <QSqlQueryModel>
#include <QTimer>
#include <Thread/syncmsgthread.h>

StackPannel::StackPannel(QWidget *parent,SELECT_UNIT* unit, bool init_msg) : BaseController(parent)
{
    this->setStyleSheet("font-family: \"Microsoft Yahei\";");
    init_info_pannel();

    target = unit;
    avatar->setPixmap(GetAvatar(unit->unit->job_number));
    name->setText(unit->unit->name);
    summary->setText(unit->unit->depname+"-"+unit->unit->groupname+"-"+unit->unit->title+"-"+unit->unit->job_number);

    init_action_pannel();
    init_chat_list();

    if(init_msg)
    {
        QTimer::singleShot(100, this, [=](){
            this->loadMsg();
        });
    }
}

//初始化顶部面板
void StackPannel::init_info_pannel()
{
    infoPannel = new QWidget(this);
    infoPannel->setObjectName("infoPannel");
    infoPannel->move(0,0);
    infoPannel->resize(1000,56);
    infoPannel->setStyleSheet("#infoPannel{background:#fff;border-bottom:1px solid #ECEEF3;}");

    avatar = new Label(infoPannel);
    avatar->setObjectName("avatar");
    avatar->resize(32,32);
    avatar->setScaledContents(true);
    avatar->move(16,12);
    QPixmap map = QPixmap::fromImage(QImage(":/Resources/User/noavatar_small.gif"));
    avatar->setPixmap(map);
    avatar->setStyleSheet("#avatar{border-radius: 10px;}");

    name = new Label(infoPannel);
    name->setObjectName("name");
    name->setText("NAME");
    name->move(62,11);
    name->setMinimumWidth(150);
    name->setStyleSheet("#name{color:#000;font-size:14px;}");

    summary = new Label(infoPannel);
    summary->setObjectName("summary");
    summary->setText("SUMMARY");
    summary->move(62,31);
    summary->setMinimumWidth(400);
    summary->setStyleSheet("#summary{color:#838690;font-size:12px;}");

    info = new Label(infoPannel);
    info->setObjectName("info");
    info->resize(20,20);
    info->setScaledContents(true);
    QPixmap infopx = QPixmap::fromImage(QImage(":/Resources/Chat/info.png"));
    info->setPixmap(infopx);
    info->setCursor(Qt::PointingHandCursor);
}

//初始化动作面板
void StackPannel::init_action_pannel()
{
    actionPannel = new QWidget(this);
    actionPannel->setObjectName("actionPannel");
    actionPannel->setStyleSheet("#actionPannel{background:#F0F3F5;}");

    action_add = new Label(actionPannel);
    action_add->setObjectName("action_add");
    action_add->resize(26,26);
    action_add->setScaledContents(true);
    QPixmap addpx = QPixmap::fromImage(QImage(":/Resources/Chat/add.png"));
    action_add->setPixmap(addpx);
    action_add->setCursor(Qt::PointingHandCursor);
    action_add->move(13,13);

    action_send = new Label(actionPannel);
    action_send->setObjectName("action_send");
    action_send->resize(26,26);
    action_send->setScaledContents(true);
    QPixmap sendpx = QPixmap::fromImage(QImage(":/Resources/Chat/send.png"));
    action_send->setPixmap(sendpx);
    action_send->setCursor(Qt::PointingHandCursor);
    connect(action_send,&Label::clicked,this,&StackPannel::send_msg);

    input = new QTextEdit(actionPannel);
    input->setObjectName("input");
    input->move(53,8);
    input->setPlaceholderText("信息");
    input->setStyleSheet("#input{background:#fff;border-bottom:1px solid #DBDDE0;border-radius:16px;padding:3px 7px;font-size:15px;}");
    input->setAcceptRichText(false);
    input->installEventFilter(this);
    connect(input,&QTextEdit::textChanged,this,&StackPannel::resize_input);
}

//初始化聊天列表
void StackPannel::init_chat_list()
{
    scrollArea = new QScrollArea(this);
    scrollArea->setObjectName("scrollAreaChatList");
    scrollArea->move(0,infoPannel->height());
    scrollArea->resize(this->width(),300);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("#scrollAreaChatList{border:0px;}");

    QString css = "#bubblelist{border:0px;background:#F0F3F5;}";
    css += "QListWidget{background-color: rgb(240, 243, 245); color:rgb(51,51,51); border: 1px solid  rgb(240, 243, 245);outline:0px;}";
    css += "QListWidget::Item{background-color: rgb(240, 243, 245);}";
    css += "QListWidget::Item:hover{background-color: rgb(240, 243, 245); }";
    css += "QListWidget::item:selected{background-color: rgb(240, 243, 245);color:black;     border: 1px solid  rgb(240, 243, 245);}";
    css += "QListWidget::item:selected:!active{border: 1px solid  rgb(240, 243, 245); background-color: rgb(240, 243, 245); color:rgb(51,51,51); }";

    bubblelist = new QListWidget(this);
    bubblelist->setObjectName("bubblelist");
    bubblelist->setStyleSheet(css);
    bubblelist->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidget(bubblelist);
}

//初始化中载入消息
void StackPannel::loadMsg()
{
    //    SyncMsgThread* t = new SyncMsgThread(target->unit->job_number);
    //    connect(t,&SyncMsgThread::sync_msg,this,[=](QString str){qDebug() << str;});
    //    t->start();
    /*
    QString sql = "select * from `MSG` where `send_from`='"+target->unit->job_number+"' or send_to='"+target->unit->job_number+"' order by id desc limit 35";
    QSqlQuery query;
    if(!query.exec(sql))
    {
        qDebug()<<query.lastError();
    }
    else
    {
        QSqlQueryModel *queryModel = new QSqlQueryModel();
        queryModel->setQuery(query);
        int nRecordCount = queryModel->rowCount();

        if(nRecordCount > 0)
        {
            SELECT_UNIT* me = new SELECT_UNIT();
            me->index = 0;
            me->job_number = user->job_number;
            me->unit = user;

            QString time = QString::number(QDateTime::currentDateTime().toTime_t());
            dealMessageTime(time);

            for (int i=0; i < nRecordCount ; i++) {
                query.next();

                AWORD* word = new AWORD();
                word->id = query.value("id").toString();
                word->from = query.value("send_from").toString();
                word->to = query.value("send_to").toString();
                word->content = query.value("content").toString();
                word->time = query.value("timestamp").toString();

                words.append(word);
            }

            for (int i=words.count()-1; i>=0; i--) {
                if(words[i]->from == target->unit->job_number)
                {
                    QNChatMessage* messageW = new QNChatMessage(target,this);
                    QListWidgetItem* item = new QListWidgetItem(bubblelist);
                    dealMessage(messageW, item, "["+words[i]->id+"]"+words[i]->content, time, QNChatMessage::User_She);
                }
                else
                {
                    QNChatMessage* messageW = new QNChatMessage(me,this);
                    QListWidgetItem* item = new QListWidgetItem(bubblelist);
                    dealMessage(messageW, item, "["+words[i]->id+"]"+words[i]->content, time, QNChatMessage::User_Me);
                }
                bubblelist->setCurrentRow(bubblelist->count()-1);
            }
            QString t = QDateTime::fromTime_t(words[0]->time.toInt()).toString("MM-dd hh:mm");
            emit post_msg(target->job_number,"text",words[0]->content,t);
        }
    }*/
}

void StackPannel::resize_input()
{
    QTextDocument *doc = input->document();
    int height = doc->size().height();
    int width = doc->size().width();
    QSize minSize = input->minimumSize();
    if (height < 36)
    {
        height = 36;
    }
    if (width != minSize.width())
    {
        width = this->width() - 100;
    }

    if(height > 250)
    {
        height = 250;
    }

    actionPannel->resize(actionPannel->width(), height + 16);
    actionPannel->move(0,this->height() - actionPannel->height());
    actionPannel->raise();

    action_add->move(13,actionPannel->height() - 39);
    action_send->move(action_send->x(),actionPannel->height() - 39);

    doc->setTextWidth(this->width() - 130);
    input->setFixedHeight(height);

    scrollArea->resize(this->width(),this->height() - infoPannel->height() - actionPannel->height());
    bubblelist->resize(scrollArea->width(),scrollArea->height());
}

//发送聊天信息
void StackPannel::send_msg()
{
    /*
    QString msg = input->toPlainText().trimmed();
    if("" != msg)
    {
        input->clear();
        input->setEnabled(false);
        if(sendmsg(target->job_number,msg))
        {
            QString time = QString::number(QDateTime::currentDateTime().toTime_t());
            dealMessageTime(time);

            QNChatMessage* messageW = new QNChatMessage(target,this);
            QListWidgetItem* item = new QListWidgetItem(bubblelist);
            dealMessage(messageW, item, msg, time, QNChatMessage::User_Me);

            bubblelist->setCurrentRow(bubblelist->count()-1);

            input->setEnabled(true);
            input->setFocus();

            int ms = get_time();

            //消息入库
            QString sql = "INSERT INTO `MSG` VALUES (NULL, 'TEXT', '"+user->job_number.toUpper()+"', '"+target->job_number.toUpper()+"', '"+msg+"', "+QString::number(ms)+", 0)";
            Db::Instance()->query(sql);

            QString meta_sql = "update `friend` set `last_msg`='"+msg+"', `last_time`="+time+" where `job_number`='"+target->job_number+"'";
            Db::Instance()->query(meta_sql);

            emit post_msg(target->job_number,"text",msg,"");
        }
        else
        {
            input->setEnabled(true);
            qDebug() << "消息发送失败了";
        }
    }*/
}

void StackPannel::reset_input()
{
    input->setEnabled(true);
    input->clear();
    input->setFocus();
}

bool StackPannel::dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time,  QNChatMessage::User_Type type)
{
    messageW->setFixedWidth(this->width());
    QSize size = messageW->fontRect(text);
    item->setSizeHint(size);
    messageW->setText(text, time, size, type);
    bubblelist->setItemWidget(item, messageW);
    return true;
}

void StackPannel::dealMessageTime(QString curMsgTime)
{
    bool isShowTime = false;
    if(bubblelist->count() > 0) {
        QListWidgetItem* lastItem = bubblelist->item(bubblelist->count() - 1);
        QNChatMessage* messageW = (QNChatMessage*)bubblelist->itemWidget(lastItem);
        int lastTime = messageW->time().toInt();
        int curTime = curMsgTime.toInt();
        isShowTime = ((curTime - lastTime) > 60);
    } else {
        isShowTime = true;
    }
    if(isShowTime) {
        QNChatMessage* messageTime = new QNChatMessage(target,bubblelist->parentWidget());
        QListWidgetItem* itemTime = new QListWidgetItem(bubblelist);

        QSize size = QSize(this->width(), 40);
        messageTime->resize(size);
        itemTime->setSizeHint(size);
        messageTime->setText(curMsgTime, curMsgTime, size, QNChatMessage::User_Time);
        bubblelist->setItemWidget(itemTime, messageTime);
    }
}

void StackPannel::paintEvent(QPaintEvent*)
{
    input->setFocus();
}

bool StackPannel::eventFilter(QObject *obj, QEvent *event)
{
    QKeyEvent *K = static_cast<QKeyEvent *>(event);

    //按下
    if(event->type() == QEvent::KeyPress)
    {
        if(Qt::Key_Control == K->key())
        {
            key_control = true;
        }
    }

    //抬起
    if(event->type() == QEvent::KeyRelease)
    {
        if(Qt::Key_Control == K->key())
        {
            key_control = false;
        }
    }

    if(event->type() == QEvent::KeyPress && obj->objectName() == "input")
    {
        if(K->key() == Qt::Key_Enter || K->key() == Qt::Key_Return)
        {
            if(key_control)
            {
                QTextCursor oldCursor = input->textCursor();
                QTextCursor textCursor(input->document());
                textCursor.setPosition(oldCursor.position());
                input->setUndoRedoEnabled(false);
                textCursor.insertBlock();
                input->setUndoRedoEnabled(true);
                return true;
            }
            else
            {
                send_msg();
                return true;
            }
        }
    }
    return false;
    //return QWidget::eventFilter( obj, event);
}

void StackPannel::new_message(QString type, QString msg)
{
    QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳
    dealMessageTime(time);

    QNChatMessage* messageW = new QNChatMessage(target,this);
    QListWidgetItem* item = new QListWidgetItem(bubblelist);
    dealMessage(messageW, item, msg, time, QNChatMessage::User_She);

    bubblelist->setCurrentRow(bubblelist->count()-1);
}

void StackPannel::resizeEvent(QResizeEvent *)
{
    infoPannel->resize(this->width(),56);
    info->move(this->width() - 30,18);

    actionPannel->resize(this->width(),52);
    actionPannel->move(0,this->height() - actionPannel->height());

    action_send->move(this->width() - action_send->width() - 10,13);

    input->resize(this->width() - 100,36);

    scrollArea->resize(this->width(),this->height() - infoPannel->height() - actionPannel->height());
    bubblelist->resize(scrollArea->width(),scrollArea->height());
}
