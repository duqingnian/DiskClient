#pragma execution_character_set("utf-8")
#include "fileexplorer.h"
#include <QDebug>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QTimer>
#include <Lib/HttpClient.h>
#include "Component/Msg.h"

FileExplorer::FileExplorer(QWidget *parent) : BaseController(parent)
{
    AccessManage = new QNetworkAccessManager(this);
    connect(AccessManage,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));

    canvas = new QWidget(this);
    canvas->setObjectName("canvas");
    canvas->setStyleSheet("#canvas{background:#ffffff;}");
    canvas->setMinimumSize(800,400);
    //canvas->setMouseTracking(true);
    //canvas->installEventFilter(this);
    canvas->setContextMenuPolicy(Qt::CustomContextMenu);

    //没有任何文件的提示
    EmptyTip = new Label(canvas);
    EmptyTip->resize(208,248);
    EmptyTip->hide();

    QLabel* EmptyIco = new QLabel(EmptyTip);
    EmptyIco->setStyleSheet("background:transparent;");
    QImage EmptyImg(":/Resources/empty_folder.png");
    EmptyIco->setPixmap(QPixmap::fromImage(EmptyImg));
    EmptyIco->move(0,0);

    QLabel* emptyTxt = new QLabel(EmptyTip);
    emptyTxt->setText("暂无文件");
    emptyTxt->setObjectName("emptyTxt");
    emptyTxt->setStyleSheet("#emptyTxt{font-size:20px;color:#605e5c;}");
    emptyTxt->setAlignment(Qt::AlignCenter);
    emptyTxt->resize(EmptyTip->width(),40);
    emptyTxt->move(0,208);

    //空白出右键
    MenuCanvas = new MenuFileExplorerCanvas(canvas);
    MenuCanvas->hide();
    connect(MenuCanvas,SIGNAL(menu_clicked(QString)),this,SLOT(menu_clicked(QString)));

    //文件右键
    fd_menu = new FdMenu(canvas);
    fd_menu->hide();
    connect(fd_menu,SIGNAL(menu_clicked(QString)),this,SLOT(fd_menu_clicked(QString)));

    flowLayout = new FlowLayout(canvas);
    flowLayout->setMargin(7);

    //新建下拉
    dlg_create = new DialogCreate(this);
    dlg_create->setVisible(false);
    connect(dlg_create,&DialogCreate::create_succ,this,[=](){
        dlg_create->setVisible(false);
        load_files();
    });

    dropdown_create = new DropDownCreate(canvas);
    dropdown_create->move(10,-5);
    dropdown_create->hide();

    //上传下拉
    dropdown_upload = new DropDownUpload(canvas);
    dropdown_upload->move(100,-5);
    dropdown_upload->hide();

    //点击新建
    connect(dropdown_create,&DropDownCreate::IntentOpenCreateDialog,this,[=](QString type){
        dropdown_upload->hide();
        dropdown_create->hide();
        dlg_create->setType(type);
        dlg_create->exec();
    });

    //点击上传
    connect(dropdown_upload,&DropDownUpload::IntentUpload,this,[=](QString IntentType){
        PrepareIntentType(IntentType);
    });
}

void FileExplorer::set_meta(UrlMeta *_meta)
{
    this->meta = _meta;
    dlg_create->setMeta(_meta);
    load_files();
}

void FileExplorer::change_folder(UrlMeta* _meta)
{
    fd->id = _meta->id;
    load_files();
}

//根据meta来获取文件
void FileExplorer::load_files()
{
    HttpClient(path("client/file.fetch")).success([=](const QString &response) {
        //qDebug() << response.toUtf8();
        //绑定文件数据到内存变量
        QJsonParseError err_rpt;
        QJsonDocument  jsonDoc = QJsonDocument::fromJson(response.toLatin1(), &err_rpt);
        if(err_rpt.error == QJsonParseError::NoError)
        {
            QJsonObject rootObj = jsonDoc.object();
            if(rootObj.contains("code"))
            {
                QJsonValue _code = rootObj.value("code");
                int code = _code.toInt();
                if(0 == code){
                    //正常加载
                    fds.clear();
                    QJsonArray arr =  rootObj.value("fds").toArray();
                    for (int i=0; i<arr.count(); i++) {
                        QJsonObject fdo = arr[i].toObject();

                        FD* fd = new FD();
                        fd->id = fdo.value("id").toInt();
                        fd->show_name = fdo.value("show_name").toString();
                        fd->type = fdo.value("type").toString();
                        fd->size = fdo.value("size").toInt();
                        fd->created_at = fdo.value("created_at").toInt();
                        fd->created_time = fdo.value("created_time").toString();
                        fd->updated_at = fdo.value("updated_at").toInt();
                        fd->updated_time = fdo.value("updated_time").toString();

                        fds.append(fd);
                    }
                    //渲染文件
                    render_files();
                }else{
                    MSGBOX::error(this,"加载文件列表时出错了!code="+QString::number(code));
                }
            }else{
                MSGBOX::error(this,"Response don't contains code!");
            }
        }else{
            MSGBOX::error(this,"QJsonParseError!加载文件列表时出错了!");
        }
    })
            .header("content-type", "application/x-www-form-urlencoded")
            .param("uid", user->uid)
            .param("meta_category", meta->category)
            .param("meta_key", meta->key)
            .param("meta_id", meta->id)
            .param("fd_id", fd->id)
            .post();
}

//resize event
void FileExplorer::flush(int width, int height)
{
    this->resize(width,height);
    canvas->resize(width,height);

    dlg_create->move((width - dlg_create->width())/2,(height - dlg_create->height())/2 -80);
    EmptyTip->move((canvas->width() - EmptyTip->width()) / 2,(canvas->height() - EmptyTip->height()) / 2 - 120);
}

//渲染文件
void FileExplorer::render_files()
{
    clear_flow_layout();

    if(fds.count() > 0){
        EmptyTip->hide();
        for (int i=0; i<fds.count(); i++) {

            int id = fds[i]->id;
            QString name = fds[i]->show_name;
            QString type = fds[i]->type;

            Label* fd_item = new Label(canvas);
            fd_item->setMinimumSize(176,171);
            fd_item->setCursor(QCursor(Qt::PointingHandCursor));
            fd_item->setObjectName("FD_"+QString::number(id));
            fd_item->setStyleSheet("#FD_"+QString::number(id)+"{background:transparent;font-family: \"Microsoft Yahei UI\";}#FD_"+QString::number(id)+":hover{background:#E5F3FF;color:#000;}");
            fd_item->setMouseTracking(true);
            fd_item->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(fd_item,&QWidget::customContextMenuRequested,this,[=](const QPoint &pos)
            {
                dropdown_upload->hide();
                dropdown_create->hide();
                MenuCanvas->hide();

                cancel_fd_selected();
                selected_fd = fds[i];

                fds[i]->selected = true;
                fd_item->setStyleSheet("#FD_"+QString::number(id)+"{background:#E5F3FF;border:1px solid #CCE8FF;;font-family: \"Microsoft Yahei UI\";}");

                fd_menu->move(fd_item->x()+pos.x(), fd_item->y()+pos.y());
                fd_menu->show();
                fd_menu->raise();
            });

            Label* fd_icon = new Label(fd_item);
            fd_icon->setStyleSheet("background:transparent;");
            fd_icon->setPixmap(QPixmap::fromImage(QImage(":/Resources/fd/"+type.toLower()+".png")));
            fd_icon->move(29,24);
            fd_icon->resize(118,86);
            fd_icon->setMouseTracking(true);

            Label* fd_num = new Label(fd_icon);
            fd_num->setStyleSheet("background:transparent;color: #bf5712;font-size: 14px;font-family: \"Microsoft Yahei UI\";");
            fd_num->setText(QString::number(id));
            fd_num->move(15,55);
            fd_num->setMouseTracking(true);

            Label* fd_name = new Label(fd_item);
            fd_name->setText(name);
            fd_name->move(0,120);
            fd_name->resize(fd_item->width(),20);
            fd_name->setAlignment(Qt::AlignCenter);
            fd_name->setStyleSheet("color:#605e5c;background:transparent;font-size: 14px;font-family: \"Microsoft Yahei UI\";");
            fd_name->setMouseTracking(true);

            Label* fd_date = new Label(fd_item);
            fd_date->setText(fds[i]->updated_time);
            fd_date->move(0,140);
            fd_date->resize(fd_item->width(),18);
            fd_date->setAlignment(Qt::AlignCenter);
            fd_date->setStyleSheet("color:#605e5c;background:transparent;font-size: 12px;font-family: \"Microsoft Yahei UI\";");

            fd_item->installEventFilter(this);

            connect(fd_icon,&Label::mouse_press,this,[=](QMouseEvent* event){this->fd_clicked(event,id);});
            connect(fd_name,&Label::mouse_press,this,[=](QMouseEvent* event){this->fd_clicked(event,id);});
            connect(fd_date,&Label::mouse_press,this,[=](QMouseEvent* event){this->fd_clicked(event,id);});
            connect(fd_item,&Label::mouse_press,this,[=](QMouseEvent* event){this->fd_clicked(event,id);});

            flowLayout->addWidget(fd_item);
        }
    }else{
        //显示没有任何文件的提示
        EmptyTip->show();
    }
}

bool FileExplorer::eventFilter(QObject *target, QEvent *event)
{
    return QWidget::eventFilter(target,event);
}

//鼠标按下
void FileExplorer::mousePressEvent(QMouseEvent *event)
{
    //不论是点击左键还是右键都会触发的
    pos = event->pos();

    //隐藏新建下拉菜单
    if(!dropdown_create->geometry().contains(pos)){
        dropdown_create->hide();
    }

    //隐藏上传下拉菜单
    if(!dropdown_upload->geometry().contains(pos)){
        dropdown_upload->hide();
    }

    //隐藏右键菜单
    if(!MenuCanvas->geometry().contains(pos)){
        MenuCanvas->hide();
    }

    //隐藏文件菜单
    if(!fd_menu->geometry().contains(pos)){
        fd_menu->hide();
    }

    //取消文件选中
    cancel_fd_selected();

    //左键
    if(Qt::LeftButton == event->button())
    {
        //
    }
    //右键
    if(Qt::RightButton == event->button())
    {
        MenuCanvas->move(pos.x(),pos.y());
        MenuCanvas->show();
        MenuCanvas->raise();
    }
}

//鼠标移动
void FileExplorer::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "移动 x=" << event->pos().x() << ",y=" << event->pos().y();
}

void FileExplorer::clear_flow_layout()
{
    QLayoutItem *child;
    while ((child = flowLayout->takeAt(0)) != 0)
    {
        if(child->widget())
        {
            child->widget()->setParent(NULL);
        }
        delete child;
    }
}

//单击文件
void FileExplorer::fd_clicked(QMouseEvent *event, int id)
{
    fd_menu->hide();
    if(Qt::LeftButton == event->button())
    {
        //qDebug() << "event->type()=" << event->type();
        if(QEvent::MouseButtonPress == event->type())
        {
            //单击
            for (int i=0; i<fds.size(); i++) {
                Label* curr_fd = findChild<Label*>("FD_"+QString::number(fds[i]->id));
                curr_fd->setStyleSheet("#FD_"+QString::number(fds[i]->id)+"{background:transparent;font-family: \"Microsoft Yahei UI\";}#FD_"+QString::number(fds[i]->id)+":hover{background:#E5F3FF;color:#000;}");

                if(fds[i]->id == id){
                    dropdown_upload->hide();
                    dropdown_create->hide();

                    selected_fd = fds[i];
                    fds[i]->selected = true;
                    curr_fd->setStyleSheet("#FD_"+QString::number(fds[i]->id)+"{background:#E5F3FF;border:1px solid #CCE8FF;;font-family: \"Microsoft Yahei UI\";}");
                }
            }
        }
        else if(QEvent::MouseButtonDblClick == event->type())
        {
            //双击打开
            fd_open();
        }
    }
}

//取消全部文件的选中状态
void FileExplorer::cancel_fd_selected()
{
    for (int i=0; i<fds.size(); i++) {
        Label* fd = findChild<Label*>("FD_"+QString::number(fds[i]->id));
        fd->setStyleSheet("#FD_"+QString::number(fds[i]->id)+"{background:transparent;font-family: \"Microsoft Yahei UI\";}#FD_"+QString::number(fds[i]->id)+":hover{background:#E5F3FF;color:#000;}");
        fds[i]->selected = false;
    }
    selected_fd = NULL;
}

//打开
void FileExplorer::fd_open()
{
    if(NULL != selected_fd)
    {
        //判断是文件还是文件夹
        if("FOLDER" == selected_fd->type)
        {
            fd = selected_fd;
            load_files();

            //如果是文件夹 需要刷新URLBAR
            emit append_urlbar(selected_fd);
        }
    }else{
        qDebug() << "selected_fd is NULL";
    }
}

//打开创建下拉框
void FileExplorer::OpenCreateDropDown()
{
    fd_menu->hide();
    MenuCanvas->hide();
    dropdown_upload->hide();

    if(dropdown_create->isVisible()){
        dropdown_create->hide();
    }else{
        dropdown_create->raise();
        dropdown_create->show();
    }
}

void FileExplorer::OpenUploadDropDown()
{
    MenuCanvas->hide();
    fd_menu->hide();
    dropdown_create->hide();

    if(dropdown_upload->isVisible()){
        dropdown_upload->hide();
    }else{
        dropdown_upload->raise();
        dropdown_upload->show();
    }
}


//处理上传
void FileExplorer::PrepareIntentType(QString IntentType)
{
    dropdown_upload->hide();

    if("file" == IntentType)
    {
        QStringList str_path_list = QFileDialog::getOpenFileNames(this,"选择上传文件","d:\\","所有文件 (*.*)");
        for (int i = 0; i < str_path_list.size(); i++){
            QString full_path = str_path_list[i]; //包含文件名称的绝对路径
            QFileInfo file = QFileInfo(full_path);
            QString file_name = file.fileName(); //文件的名称

            upload_queue.append(full_path);
            upload_array.append(file_name);
        }
        //开始上传
        show_upload_pannel();
    }
    else if("folder" == IntentType)
    {
        qDebug() << "上传文件夹";
    }
    else if("drag" == IntentType)
    {
        qDebug() << "拖动上传";
    }
    else
    {
        //do nothing....
    }
}

void FileExplorer::menu_clicked(QString key)
{
    QTimer::singleShot(50, this, [=](){
        //刷新
        if("refresh" == key)
        {
            load_files();
        }
        //创建文件夹
        if("cfolder" == key)
        {

            dlg_create->setType("folder");
            dlg_create->exec();

        }
        //创建TXT文件
        if("ctxt" == key)
        {
            QTimer::singleShot(50, this, [=](){
                dlg_create->setType("txt");
                dlg_create->exec();
            });
        }
        //上传文件
        if("upload_file" == key)
        {
            PrepareIntentType("file");
        }
        //上传文件夹
        if("upload_folder" == key)
        {
            PrepareIntentType("folder");
        }

    });
    fd_menu->hide();
    MenuCanvas->hide();
    dropdown_upload->hide();
    dropdown_create->hide();
}

void FileExplorer::fd_menu_clicked(QString key)
{
    qDebug() << "fd_menu_clicked key=" << key;
    fd_menu->hide();

    if("open" == key)
    {
        fd_open();
    }

}

void FileExplorer::replyFinished(QNetworkReply * _reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        qDebug() << "read all:" << _reply->readAll();
        reply->deleteLater();
        handle->flush();
        handle->close();

        qDebug() << "上传结束了";
    }
    else
    {
        //出错了
    }
}

void FileExplorer::loadError(QNetworkReply::NetworkError)
{
    qDebug()<<"Error: "<<reply->error();
}

void FileExplorer::loadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    qDebug() << "loaded" << bytesSent << "of" << bytesTotal;
    //progressBar->setMaximum(bytesTotal); //最大值
    //progressBar->setValue(bytesSent);  //当前值
}

//显示上传面板
void FileExplorer::show_upload_pannel()
{
    //QString url = path("/client/file/upload");
    QString header = "MSGTYPE:FILE|FROM:"+user->job_number+"|TO:SYS";

    QTcpSocket* socket = Socket::Instance()->handle();
    for (int i=0; i<upload_queue.count(); i++) {
        QTimer::singleShot(100, this, [=](){
            //
        });
    }

}





























