#pragma execution_character_set("utf-8")
#include "fileexplorer.h"
#include "filerow.h"
#include <QDebug>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMovie>
#include <QThread>
#include <QTimer>
#include <Lib/HttpClient.h>
#include <Thread/processdirthread.h>
#include "Component/Msg.h"

#define FD_WIDTH 176
#define FD_HEIGHT 171
#define FIOW_LAYOUT_MARGIN 7
#define LIST_SIDE_WIDTH 345

FileExplorer::FileExplorer(QWidget *parent) : BaseController(parent)
{
    typesRes << "null" << "folder"<<"msi";
    typesRes << "js" << "css" << "html" << "xml" << "ini";
    typesRes << "mp3" << "mp4" << "avi" << "rm" << "wmv" << "mov" << "3gp" << "flac" << "wav" << "ncm"; //音频视频
    typesRes << "bmp" << "psd" << "jpeg" << "jpg" << "png" << "gif" << "svg";  //图像
    typesRes <<  "doc" << "docx" << "csv" << "ppt" << "xls" << "xlsx" << "pptx";
    typesRes <<  "7z" << "rar" << "zip";
    typesRes <<  "ai" << "pdf" << "fl";
    typesRes <<  "php" << "cpp" << "py";
    typesRes <<  "aep" << "bat" << "email" << "emf" << "eps" << "cdr" << "exe" << "iso" << "raw" << "swf" << "tif" << "ttf" << "epub";
    typesRes <<  "txt" << "wmf" << "ico" << "chm" << "dll" << "sql" << "log";

    //文件socket
    QString file_server_ip   = get_reg("file_server");
    QString file_server_port = get_reg("file_port");

    file_socket = new QTcpSocket();
    file_socket->connectToHost(file_server_ip,file_server_port.toInt());

    connect(file_socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(file_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    //没有任何文件的提示
    EmptyTip = new Label(this);
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

    canvas = new QWidget(this);
    canvas->setObjectName("canvas");
    canvas->setStyleSheet("#canvas{background:#ffffff;}");
    canvas->setMinimumSize(800,400);
    canvas->setContextMenuPolicy(Qt::CustomContextMenu);
    canvas->move(0,0);

    //新建下拉
    dlg_create = new DialogCreate(this);
    dlg_create->setVisible(false);
//    connect(dlg_create,&DialogCreate::create_succ,this,[=](){
//        dlg_create->setVisible(false);
//        load_files();
//    });
    connect(dlg_create,&DialogCreate::intent,this,[=](QString intent_name,QString intent_category, QString val){
        qDebug() << intent_name << intent_category << val;
        sendmsg("SYS","","")
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

    upload_pannel = new UploadPannel(this);
    upload_pannel->set_descriptor(file_socket->socketDescriptor());
    upload_pannel->setObjectName("upload_pannel");
    upload_pannel->setStyleSheet("#upload_pannel{background:#fff;border: 1px solid #ddd;border-radius: 5px;}");
    upload_pannel->resize(495,375);
    upload_pannel->move(this->width() - upload_pannel->width() - 15,this->height() - upload_pannel->height() - 15);
    upload_pannel->hide();

    connect(upload_pannel,&UploadPannel::do_some_action,this,[=](QString action){
        if("CLOSE" == action)
        {
            upload_pannel->hide();
        }
        else if("MIN" == action)
        {
            upload_pannel->resize(495,38);
            upload_pannel->move(this->width() - upload_pannel->width() - 15,this->height() - 38 - 15);
        }
        else if("MAX" == action)
        {
            upload_pannel->resize(495,375);
            upload_pannel->move(this->width() - upload_pannel->width() - 15,this->height() - upload_pannel->height() - 15);
        }
        else{}
    });

    //空白右键
    MenuCanvas = new MenuFileExplorerCanvas(canvas);
    MenuCanvas->hide();
    connect(MenuCanvas,SIGNAL(menu_clicked(QString)),this,SLOT(menu_clicked(QString)));

    //文件右键
    fd_menu = new FdMenu(canvas);
    fd_menu->hide();
    connect(fd_menu,SIGNAL(menu_clicked(QString)),this,SLOT(fd_menu_clicked(QString)));

    if("FLOW" == DisplayMod)
    {
        flowLayout = new FlowLayout(canvas);
        flowLayout->setMargin(FIOW_LAYOUT_MARGIN);

        scrollArea = new QScrollArea(this);
        scrollArea->setObjectName("scrollAreaChatList");
        scrollArea->move(0,0);
        scrollArea->resize(this->width(),300);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setStyleSheet("#scrollAreaChatList{border:0px;}");
        scrollArea->setWidget(canvas);
    }
    else if("LIST" == DisplayMod)
    {
        render_list_header();

        QString css = "#listWapper{border:0px;background:#fff;}";
        listWapper = new QListWidget(canvas);
        listWapper->setObjectName("listWapper");
        listWapper->setStyleSheet(css);
        listWapper->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWapper->resize(this->width() - LIST_SIDE_WIDTH,400);
        listWapper->move(0,35);

        ListSide = new QWidget(canvas);
        ListSide->setObjectName("ListSide");
        ListSide->setStyleSheet("#ListSide{background:#fff;border-left:1px solid #d8dee4;}");
        ListSide->resize(LIST_SIDE_WIDTH,400);

        connect(listWapper,&QListWidget::itemClicked,this,&FileExplorer::row_clicked);
        connect(listWapper,&QListWidget::itemDoubleClicked,this,&FileExplorer::row_db_clicked);
    }
    else{}

    mask = new QWidget(this);
    mask->setObjectName("mask");
    mask->resize(1000,500);
    mask->move(0,0);
    mask->setStyleSheet("#mask{background:transparent;}");
    mask->raise();

    load = new QLabel(mask);
    load->resize(60,60);

    dir_process = new DirProcess(mask);
    dir_process->setObjectName("dir_process");
    dir_process->setStyleSheet("#dir_process{border-width: 16px;border-image: url(:/Resources/shadow_wnd_bg.png) 16 16 16 16;}");
    dir_process->hide();

    connect(dir_process,&DirProcess::do_something,this,[=](QString name){
        if("CANCEL" == name)
        {
            //取消
            dir_process->hide();
            mask->hide();
        }
        else{}
    });

}

void FileExplorer::readyRead()
{
    QByteArray buffer;
    QDataStream socketStream(file_socket);
    socketStream.setVersion(QDataStream::Qt_5_15);
    socketStream.startTransaction();
    socketStream >> buffer;
    file_socket->flush();
    if(!socketStream.commitTransaction())
    {
        return;
    }
    buffer = buffer.mid(0,buffer.indexOf(";"));

    if("DQN|" != buffer.mid(0,4))
    {
        return;
    }

    QString _header = buffer.mid(4);

    QString META = "";

    QStringList headers = _header.split(",");
    for(QString hi : headers)
    {
        QStringList his = hi.split(":");
        if("META" == his[0]) //标识符
        {
            META = his[1];
        }
        else{}
    }

    if("WELCOME" == META)
    {
        //欢迎消息
        qDebug() << "文件服务器在线中...";
    }
    else
    {}
}

void FileExplorer::disconnected()
{
    file_socket->abort();
    file_socket->deleteLater();
}

void FileExplorer::render_list_header()
{
    file_row_header = new QWidget(canvas);
    file_row_header->setObjectName("file_row_header");
    file_row_header->setStyleSheet("#file_row_header{background:#F8F9F9;border-bottom:1px solid #E7E7E7;color:#000;}");
    file_row_header->resize(this->width() - LIST_SIDE_WIDTH,35);
    file_row_header->move(0,0);

    Label* row_header_ico = new Label(file_row_header);
    row_header_ico->resize(50,35);
    row_header_ico->move(0,0);

    Label* row_header_name = new Label(file_row_header);
    row_header_name->resize(520,35);
    row_header_name->move(55,0);
    row_header_name->setText("名称");
    row_header_name->setStyleSheet("font-weight:bold;");

    Label* row_header_user = new Label(file_row_header);
    row_header_user->resize(180,35);
    row_header_user->move(585,0);
    row_header_user->setText("用户");
    row_header_user->setStyleSheet("font-weight:bold;");

    Label* row_header_type = new Label(file_row_header);
    row_header_type->resize(130,35);
    row_header_type->move(780,0);
    row_header_type->setText("文件类型");
    row_header_type->setStyleSheet("font-weight:bold;");

    Label* row_header_size = new Label(file_row_header);
    row_header_size->resize(90,35);
    row_header_size->move(910,0);
    row_header_size->setText("文件尺寸");
    row_header_size->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    row_header_size->setStyleSheet("font-weight:bold;");
}

void FileExplorer::show_loading()
{
    QMovie *movie = new QMovie(":/Resources/load.gif");
    load->setMovie(movie);
    movie->start();
    mask->show();
    load->show();
}

void FileExplorer::hide_loading()
{
    mask->hide();
    load->hide();
}

FileExplorer::~FileExplorer()
{
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
    this->clear();
    show_loading();
    mutex.lock();
    qDebug() << "###" << path("client/file.fetch");
    HttpClient(path("client/file.fetch")).success([=](const QString &response) {
        qDebug() << response.toUtf8();
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
                        QString ico_res = ":/Resources/types/"+fd->type.toLower()+".png";
                        if(typesRes.indexOf(fd->type.toLower()) == -1)
                        {
                            qDebug() << "UnKnow File type:" << fd->type.toLower();
                            ico_res = ":/Resources/types/null.png";
                        }
                        fd->icon = ico_res;

                        fds.append(fd);
                    }
                    //渲染文件
                    render_files();

                    mutex.unlock();
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
    _width = width;
    _height = height;

    this->resize(width,height);
    canvas->resize(width,height);
    mask->resize(width,height);

    if("FLOW" == DisplayMod)
    {
        wait(10);
        calculate_flow_layout_height();

        wait(10);
        scrollArea->resize(width-2,height);
        load->move((mask->width()-60)/2,(mask->height()-60)/2);
    }
    else if("LIST" == DisplayMod)
    {
        listWapper->resize(_width - LIST_SIDE_WIDTH,height-35);
        file_row_header->resize(_width - LIST_SIDE_WIDTH,35);

        ListSide->move(_width - LIST_SIDE_WIDTH,0);
        ListSide->resize(LIST_SIDE_WIDTH,_height);

        load->move((listWapper->width()-60)/2,(listWapper->height()-60)/2);
    }
    dlg_create->move((width - dlg_create->width())/2,(height - dlg_create->height())/2 -80);
    EmptyTip->move((canvas->width() - EmptyTip->width()) / 2,(canvas->height() - EmptyTip->height()) / 2 - 120);
    upload_pannel->move(this->width() - upload_pannel->width() - 15,this->height() - upload_pannel->height() - 15);

    dir_process->move((listWapper->width()-dir_process->width())/2,(listWapper->height()-dir_process->height())/2);
}

void FileExplorer::row_clicked(QListWidgetItem *item)
{
    db_click = false;

    dropdown_upload->hide();
    dropdown_create->hide();
    fd_menu->hide();

    int id = item->data(Qt::UserRole).toInt();
    for (int i=0; i<fds.size(); i++) {
        if(fds[i]->id == id){
            dropdown_upload->hide();
            dropdown_create->hide();

            selected_fd = fds[i];
            fds[i]->selected = true;
        }
    }
}

void FileExplorer::row_db_clicked(QListWidgetItem *item)
{
    db_click = true;
    fd_open();
}

void FileExplorer::Refresh()
{
    qDebug() << "Refresh....";
    load_files();
}

void FileExplorer::calculate_flow_layout_height(bool hs)
{
    if(fds.count() < 1)
    {
        return;
    }
    int row_items = this->_width / 183;
    int rows = fds.count() / row_items;
    if(fds.count() % row_items > 0)
    {
        rows++;
    }
    if(hs)
    {
        int h = rows * (FD_HEIGHT + FIOW_LAYOUT_MARGIN);
        if(h < _height)
        {
            h = _height;
        }
        canvas->resize(canvas->width(),h);
    }
}

//渲染文件
void FileExplorer::render_files()
{
    if("FLOW" == DisplayMod)
    {
        this->render_flow();
    }
    else if("LIST" == DisplayMod)
    {
        this->render_list();
    }
    else
    {
        qDebug() << "UnKnow DisplayMod:" << DisplayMod;
    }
    hide_loading();
}

void FileExplorer::clear()
{
    fds.clear();
    if("FLOW" == DisplayMod)
    {
        clear_flow_layout();
    }
    else if("LIST" == DisplayMod)
    {
        cleat_list_rows();
    }
    else
    {}
}

void FileExplorer::render_list()
{
    cleat_list_rows();
    if(fds.count() > 0){
        EmptyTip->hide();
        for (int i=0; i<fds.count(); i++)
        {
            QString id = QString::number(fds[i]->id);

            FileRow* row = new FileRow();
            row->setObjectName("file_row_"+id);
            row->set_file(fds[i]);

            QListWidgetItem* item = new QListWidgetItem(listWapper);
            item->setData(Qt::UserRole,id);
            item->setSizeHint(QSize(1020,50));

            listWapper->setItemWidget(item, row);

            row->setMouseTracking(true);
            row->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(row,&QWidget::customContextMenuRequested,this,[=](const QPoint &pos)
            {
                //列表形式的右键
                dropdown_upload->hide();
                dropdown_create->hide();
                MenuCanvas->hide();

                selected_fd = fds[i];
                fds[i]->selected = true;

                fd_menu->move(row->x()+pos.x(), row->y()+pos.y()+35);
                fd_menu->show();
                fd_menu->raise();
            });
        }
    }
}

void FileExplorer::render_flow()
{
    clear_flow_layout();
    if(fds.count() > 0){
        EmptyTip->hide();
        for (int i=0; i<fds.count(); i++) {

            int id = fds[i]->id;
            QString name = fds[i]->show_name;
            QString type = fds[i]->type;

            Label* fd_item = new Label(canvas);
            fd_item->setMinimumSize(FD_WIDTH,FD_HEIGHT);
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

            QString ico_res = ":/Resources/types/"+type.toLower()+".png";
            if(typesRes.indexOf(type.toLower()) == -1)
            {
                qDebug() << "UnKnow File type:" << type.toLower();
                ico_res = ":/Resources/types/null.png";
            }

            Label* fd_icon = new Label(fd_item);
            fd_icon->setStyleSheet("background:transparent;");
            fd_icon->setPixmap(QPixmap::fromImage(QImage(ico_res)));
            fd_icon->move(45,24);
            fd_icon->resize(86,86);
            fd_icon->setScaledContents(true);
            fd_icon->setMouseTracking(true);

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
        calculate_flow_layout_height();
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
    qDebug() << "mousePressEvent 鼠标按下......";
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
    if("FLOW" == DisplayMod)
    {
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
}

//鼠标移动
void FileExplorer::mouseMoveEvent(QMouseEvent *)
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

void FileExplorer::cleat_list_rows()
{
    //QListWidgetItem *child;
    listWapper->clear();
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
    int version = 1;

    if(1 == version)
    {
        for (int i=0; i<fds.size(); i++) {
            Label* fd = findChild<Label*>("FD_"+QString::number(fds[i]->id));
            fd->setStyleSheet("#FD_"+QString::number(fds[i]->id)+"{background:transparent;font-family: \"Microsoft Yahei UI\";}#FD_"+QString::number(fds[i]->id)+":hover{background:#E5F3FF;color:#000;}");
            fds[i]->selected = false;
        }
        selected_fd = NULL;
    }else if(2 == version)
    {
        Label* fd = findChild<Label*>("FD_"+QString::number(selected_fd->id));
        fd->setStyleSheet("#FD_"+QString::number(selected_fd->id)+"{background:transparent;font-family: \"Microsoft Yahei UI\";}#FD_"+QString::number(selected_fd->id)+":hover{background:#E5F3FF;color:#000;}");
        selected_fd->selected = false;
        selected_fd = NULL;
    }else{}
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

        if(str_path_list.size() > 99)
        {
            box("一次最多只能选择99个文件!你选择了:"+QString::number(str_path_list.size())+"个");
            return;
        }

        for (int i = 0; i < str_path_list.size(); i++){
            QString abs_path = str_path_list[i]; //包含文件名称的绝对路径
            this->moveto_queue(abs_path);
        }
        upload_pannel->show();
        upload_pannel->raise();
        upload_pannel->touch_upload();
    }
    else if("folder" == IntentType)
    {
        qDebug() << "上传文件夹";
        upload_dir = QFileDialog::getExistingDirectory(this, "请选择目录", "D:\\");

        if (upload_dir.isEmpty())
        {
            return;
        }
        else
        {
            mask->show();
            dir_process->init(fd->id);
            dir_process->show();
            dir_process->raise();

            dir_process->set_data("dir_name",upload_dir);
            dir_process->set_data("dir_path",upload_dir);

            //开启一个线程去去处理文件数量和尺寸
            ProcessDirThread* t = new ProcessDirThread();
            t->set_dir(upload_dir);
            t->start();

            //发现了文件和文件夹
            connect(t,&ProcessDirThread::find_file,this,&FileExplorer::append_file);
            connect(t,&ProcessDirThread::sync_size,this,[=](unsigned long long size){
                dir_process->set_data("ADD_SIZE",QString::number(size));
            });

            connect(t,&ProcessDirThread::process_complete,dir_process,[=](){
                dir_process->set_data("PREPARE_UPLOAD","");
                dir_process->init(0);
                mask->hide();
                dir_process->hide();

                upload_pannel->show();
                upload_pannel->raise();
                upload_pannel->touch_upload();
            });
        }
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

void FileExplorer::moveto_queue(QString abs_path)
{
    QFileInfo file = QFileInfo(abs_path);
    QString file_name = file.fileName();
    unsigned long long fize_size = file.size();

    UP_FILE* upload_file = new UP_FILE();
    upload_file->suffix = "";
    upload_file->ico = "";

    if(file_name.contains("."))
    {
        upload_file->suffix = file_name.mid(file_name.lastIndexOf(".")+1).toLower();
        upload_file->ico = upload_file->suffix;

        if(typesRes.indexOf(upload_file->suffix) == -1)
        {
            upload_file->ico = "null";
        }
    }
    upload_file->md5 = md5(abs_path);
    upload_file->name = file_name;
    upload_file->path = abs_path;
    upload_file->size = fize_size;
    upload_file->state = UP_STATE::WAIT_UP;
    wait(5);
    upload_pannel->add_queue(upload_file);
}

void FileExplorer::append_file(QString T, QString abs_file)
{
    if("FILE" == T)
    {
        dir_process->set_data("ADD_FILE","");
        moveto_queue(abs_file);
    }
    else if("DIR" == T)
    {
        dir_process->set_data("ADD_DIR","");
    }
    else
    {}
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


