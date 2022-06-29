#pragma execution_character_set("utf-8")
#include "employeerow.h"
#include "fileexplorer.h"
#include "filerow.h"
#include <QDebug>
#include <QJsonArray>

#define FD_WIDTH 176
#define FD_HEIGHT 171
#define FIOW_LAYOUT_MARGIN 7
#define LIST_SIDE_WIDTH1 345
#define LIST_SIDE_WIDTH2 512
#define DATA_META_LEN 256
#define SIDE_MIN_ACTIONS_BAR_HEIGHT 0

FileExplorer::FileExplorer(QWidget *parent) : BaseController(parent)
{
    //0-9
    typesRes << "3gp" << "7z";
    //A
    typesRes << "avi" << "ai" <<  "aep" ;
    //B
    typesRes << "bmp" << "bat";
    //C
    typesRes << "css" << "csv" << "cpp" << "cdr" << "chm"  << "cs";
    //D
    typesRes << "doc" << "docx" << "dll";
    //E
    typesRes << "email" << "emf" << "eps" << "exe" << "epub";
    //F
    typesRes << "folder" << "flac" << "fl" << "flv";
    //G
    typesRes << "gif";
    //H
    typesRes << "html";
    //I
    typesRes << "ini" << "iso" << "ico";
    //J
    typesRes << "js" << "jpeg" << "jpg";
    //K
    //typesRes << "";
    //L
    typesRes << "log";
    //M
    typesRes << "msi" << "mp3" << "mp4" << "mov";
    //N
    typesRes << "null" << "ncm";
    //O
    //typesRes << "";
    //P
    typesRes << "psd" << "png" << "ppt" << "pptx" << "pdf" <<  "php" << "py";
    //Q
    //typesRes << "";
    //R
    typesRes << "rm" << "rar" << "raw" << "reg";
    //S
    typesRes << "svg" << "swf" << "sql";
    //T
    typesRes << "tif" << "ttf" <<  "txt";
    //U
    //typesRes << "";
    //V
    //typesRes << "";
    //W
    typesRes << "wmv" << "wav" << "wmf";
    //X
    typesRes << "xml" << "xls" << "xlsx";
    //Y
    //typesRes << "";
    //Z
    typesRes << "zip";

    selected_fd = new FD();
    selected_fd->id = 0;

    active_fd = new FD();
    active_fd->id = 0;

    SJN = "";

    QString watch_suffix = get_reg("WATCH_SUFFIX");
    if("" == watch_suffix)
    {
        watch_suffix = "doc,docx,xls,xlsx,csv,ppt,pptx,psd";
    }
    office = watch_suffix.split(",");

    MAX_UPLOAD_FILES = get_reg("MAX_UPLOAD_FILES").toInt();
    if(MAX_UPLOAD_FILES < 99 || MAX_UPLOAD_FILES > 999)
    {
        MAX_UPLOAD_FILES = 99;
    }

    //文件socket
    QString file_server_ip   = get_reg("file_server");
    QString file_server_port = get_reg("file_port");

    cmd_socket = new QTcpSocket();
    cmd_socket->connectToHost(file_server_ip,file_server_port.toInt());

    connect(cmd_socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(cmd_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    if(cmd_socket->waitForConnected())
    {
        if(cmd_socket->isOpen())
        {
            //qDebug() << "cmd服务器连接成功";
            //上传面板
            upload_pannel = new UploadPannel(this);
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
                else if("REFRESH" == action)
                {
                    load_files();
                }
                else
                {}
            });
        }
        else
        {
            box("无法连接到cmd服务器");
        }
    }
    else
    {
        box("cmd服务器连接失败!");
    }
    upload_socket_descriptor = upload_pannel->GetSocketDescriptor();

    download_socket = new QTcpSocket();
    download_socket->connectToHost(file_server_ip,file_server_port.toInt());

    connect(download_socket, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()), Qt::DirectConnection);
    if(download_socket->waitForConnected())
    {
        if(download_socket->isOpen())
        {
            //成功连接到了下载服务器
        }
        else
        {
            box("无法连接到下载服务器");
        }
    }
    else
    {
        box("下载服务器连接失败!");
    }

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

    //对应的创建操作
    dlg_create = new DialogCreate(this);
    dlg_create->setVisible(false);
    connect(dlg_create,&DialogCreate::intent,this,[=](QString intent_name,QString intent_category, QString val){
        sendMsgPack("INTENT:?BUNDLE="+this->meta->key+",BUNDLE_ID="+QString::number(this->meta->id)+",META="+intent_name+",CATE="+intent_category+",VAL="+val + ",SJN="+SJN+",FD=" + QString::number(fd->id),"",cmd_socket);
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
        reload_album = new Label(canvas);
        reload_album->setObjectName("reload_album");
        reload_album->setStyleSheet("#reload_album{background:#F8F9F9;font-size:14px;border-right:1px solid #E7E7E7;border-bottom:1px solid #E7E7E7;}");
        reload_album->setAlignment(Qt::AlignCenter);
        reload_album->move(0,0);
        reload_album->setCursor(Qt::PointingHandCursor);

        connect(reload_album,&Label::clicked,this,[=](){
            SJN = "";
            emplyees->clearSelection();
            emit reload_meta_data(meta);
        });

        emplyees = new QListWidget(canvas);
        emplyees->setObjectName("emplyees");
        emplyees->setStyleSheet("#emplyees{background:#fff;border:0px;}");
        emplyees->resize(0,400);
        emplyees->move(0,35);
        emplyees->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        connect(emplyees,&QListWidget::itemClicked,this,&FileExplorer::select_employee);

        render_list_header();

        QString css = "#listWapper{border:0px;background:#fff;border-left:1px solid #E7E7E7;}";
        listWapper = new QListWidget(canvas);
        listWapper->setObjectName("listWapper");
        listWapper->setStyleSheet(css);
        listWapper->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWapper->resize(this->width() - LIST_SIDE_WIDTH1,400);
        listWapper->move(0,35);
        listWapper->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

        ListSide = new QWidget(canvas);
        ListSide->setObjectName("ListSide");
        ListSide->setStyleSheet("#ListSide{background:#fff;border-left:1px solid #d8dee4;}");
        ListSide->resize(LIST_SIDE_WIDTH1,400);

        connect(listWapper,&QListWidget::itemClicked,this,&FileExplorer::row_clicked);
        connect(listWapper,&QListWidget::itemDoubleClicked,this,&FileExplorer::row_db_clicked);
        connect(listWapper,&QListWidget::itemSelectionChanged,this,&FileExplorer::row_item_section_changed);

        FileExplorer::render_list_side();
    }
    else{}

    mask = new QWidget(this);
    mask->setObjectName("mask");
    mask->resize(1000,500);
    mask->move(0,0);
    mask->setStyleSheet("#mask{background-color: transparent;}");
    mask->raise();

    load = new QLabel(mask);
    load->resize(60,60);

    loading_tip = new QLabel(mask);
    loading_tip->setObjectName("loading_tip");
    loading_tip->setStyleSheet("#loading_tip{font-size:14px;color:#000;}");
    loading_tip->resize(200,25);
    loading_tip->setAlignment(Qt::AlignCenter);

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

    //开始判断目录
    USR_CACHE_DIR = get_reg("LOCAL_CACHE_DIR")+"\\"+user->job_number;
    QDir cache_dir(USR_CACHE_DIR);
    if(!cache_dir.exists())
    {
        //尝试创建
        if(!cache_dir.mkpath(USR_CACHE_DIR))
        {
            box("文件缓存目录丢失，并且创建失败！请检查注册表配置项.PATH="+USR_CACHE_DIR);
        }
    }
}

void FileExplorer::flush(int width, int height)
{
    _width = width;
    _height = height;

    int LIST_SIDE_WIDTH = LIST_SIDE_WIDTH1;
    if(width > 1400)
    {
        LIST_SIDE_WIDTH = LIST_SIDE_WIDTH2;
    }
    else
    {
        LIST_SIDE_WIDTH = LIST_SIDE_WIDTH1;
    }

    this->resize(width,height);
    canvas->resize(width,height);
    mask->resize(width,height);
    active_title->resize(LIST_SIDE_WIDTH-20,25);

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
        //是不是部门，是部门的话 就显示部门的成员列表
        if("MY_FILE" == meta->key)
        {
            emp_width = 0;
        }
        else
        {
            if("SHOW" == SHOW_EMP)
            {
                emp_width = 120;
            }
            else
            {
                emp_width = 0;
            }
        }
        reload_album->resize(emp_width,35);
        emplyees->resize(emp_width,height-35);

        listWapper->resize(_width - LIST_SIDE_WIDTH - emp_width,height-35);
        file_row_header->resize(_width - LIST_SIDE_WIDTH - emp_width,35);
        file_row_header->move(emp_width,0);

        listWapper->move(emp_width,35);
        ListSide->move(_width - LIST_SIDE_WIDTH,0);
        ListSide->resize(LIST_SIDE_WIDTH,_height);

        side_tab->resize(LIST_SIDE_WIDTH-20,_height-(210+SIDE_MIN_ACTIONS_BAR_HEIGHT));
        file_min_action->move(10,ListSide->height()-(SIDE_MIN_ACTIONS_BAR_HEIGHT+10));
        file_min_action->resize(LIST_SIDE_WIDTH-20,SIDE_MIN_ACTIONS_BAR_HEIGHT);

        load->move((listWapper->width()-60)/2,(listWapper->height()-60)/2);
        loading_tip->move(load->x() - loading_tip->width()/2 + 30,load->y() + 90);
    }
    dlg_create->move((width - dlg_create->width())/2,(height - dlg_create->height())/2 -80);
    EmptyTip->move((canvas->width() - EmptyTip->width()) / 2,(canvas->height() - EmptyTip->height()) / 2 - 120);
    upload_pannel->move(this->width() - upload_pannel->width() - 15,this->height() - upload_pannel->height() - 15);
    active_icon->move((LIST_SIDE_WIDTH-120)/2,20);
    dir_process->move((listWapper->width()-dir_process->width())/2,(listWapper->height()-dir_process->height())/2);

    //文件列表
    if(width > 1400)
    {
        row_header_name->resize(865,35);
    }
    else
    {
        row_header_name->resize(475,35);
    }
    row_header_user->move(row_header_name->x() + row_header_name->width() + 10,0);
    row_header_version->move(row_header_user->x() + row_header_user->width(),0);
    row_header_size->move(row_header_version->x() + row_header_version->width(),0);

    for (int i=0; i<fds.count(); i++)
    {
        QString id = QString::number(fds[i]->id);
        FileRow* row = findChild<FileRow*>("file_row_"+id);
        row->resize(width);
    }
}

//渲染文件详情
void FileExplorer::render_list_side()
{
    active_icon = new QLabel(ListSide);
    active_icon->setObjectName("active_icon");
    active_icon->setStyleSheet("#active_icon{background:#fff;}");
    active_icon->setPixmap(QPixmap::fromImage(QImage(":/Resources/types/folder.png")));
    active_icon->resize(120,120);
    active_icon->setScaledContents(true);
    active_icon->move((LIST_SIDE_WIDTH1-120)/2,20);

    active_title = new QLabel(ListSide);
    active_title->setObjectName("active_title");
    active_title->setStyleSheet("#active_title{font-size:20px;background:#fff;}");
    active_title->resize(LIST_SIDE_WIDTH1-20,25);
    active_title->setText("标题");
    active_title->setAlignment(Qt::AlignCenter);
    active_title->move(10,155);

    //侧边栏TAB
    side_tab = new QTabWidget(ListSide);
    side_tab->move(10,190);
    side_tab->resize(LIST_SIDE_WIDTH1-20,400);
    side_tab->setObjectName("side_tab");
    side_tab->setStyleSheet("#side_tab{background:#fff;}QTabBar::tab{font-size:14px;background:#fff;height: 44px;width:100px;text-align:center;}QTabBar::tab:selected{font-weight:700;border-top:2px solid rgb(0, 120, 212);border-left:1px solid #D9D9D9;border-right:1px solid #D9D9D9;}");

    this->render_base_info();
    this->render_version_list();
}

void FileExplorer::downloadReadyRead()
{
    QByteArray buffer;

    QDataStream socketStream(download_socket);
    socketStream.setVersion(QDataStream::Qt_5_15);
    socketStream.startTransaction();
    socketStream >> buffer;
    download_socket->flush();

    if(!socketStream.commitTransaction())
    {
        return;
    }

    QByteArray data = buffer.mid(DATA_META_LEN);
    QByteArray header = buffer.mid(0,buffer.indexOf(";"));
    if("DQN:?" != header.mid(0,5))
    {
        return;
    }

    QString _header = header.mid(5);

    QString META = "";
    int     CODE = 0;
    QString MSG  = "";
    int     FD_ID  = 0;

    QStringList headers = _header.split(",");
    for(QString hi : headers)
    {
        QStringList his = hi.split(":");
        if("META" == his[0]) //标识符
        {
            META = his[1];
        }
        else if("CODE" == his[0])
        {
            CODE = his[1].toInt();
        }
        else if("MSG" == his[0])
        {
            MSG = his[1];
        }
        else if("FD_ID" == his[0])
        {
            FD_ID = his[1].toInt();
        }
        else{}
    }
    if("DOWNLOAD" == META)
    {
        //接收文件 DOWNLOAD:?FILE_ID:%1,MD5:%2,LEFT_SIZE:%3,BUF_SIZE:%4,FS:%5,PCT:%6;
        /////////////////////////////////////////////////////////////////////
        QString FILE_ID ="";
        QString MD5 = "";
        QString LEFT_SIZE = "";
        QString BUF_SIZE = "";
        QString FS = "";
        QString PCT = "";
        QString NAME = "";
        int OPEN = 0;

        QStringList headers = _header.split(",");
        for(int i=0;i<headers.length();i++)
        {
            QString hi = headers[i];
            QStringList his = hi.split(":");
            if("MD5" == his[0])
            {
                MD5 = his[1];
            }
            else if("FILE_ID" == his[0])
            {
                FILE_ID = his[1];
            }
            else if("LEFT_SIZE" == his[0])
            {
                LEFT_SIZE = his[1];
            }
            else if("BUF_SIZE" == his[0])
            {
                BUF_SIZE = his[1];
            }
            else if("FS" == his[0])
            {
                FS = his[1].toLower();
            }
            else if("PCT" == his[0])
            {
                PCT = his[1];
            }
            else if("NAME" == his[0])
            {
                NAME = his[1];
            }
            else if("OPEN" == his[0])
            {
                OPEN = his[1].toInt();
            }
            else{}
        }
        if("0" == FILE_ID)
        {
            box("FILE_ID不能为0");
            return;
        }
        if(LEFT_SIZE.length() > 0 && LEFT_SIZE.toInt() > 0)
        {
            //do this code
        }
        else
        {
            return;
        }
        //查找文件  E:\DISK_CACHE\CZML669\86\cd1556ac610559c2ee333b8669557d3f
        QString LOCAL_CACHE_DIR = get_reg("LOCAL_CACHE_DIR");

        QString save_path(LOCAL_CACHE_DIR+"\\"+user->job_number+"\\"+FILE_ID+"\\"+MD5);
        QDir save_dir(save_path);
        if(!save_dir.exists())
        {
            save_dir.mkpath(save_path);
        }

        QString tmp_file = save_path+"\\"+NAME+".tmp";
        QFile file(tmp_file);

        if(!file.exists())
        {
            if(file.open(QIODevice::WriteOnly)){
                show_loading("准备下载中...");
                if(FS.length() <=6 && FS.toUInt() <= BUF_SIZE.toULongLong())
                {
                    file.write(buffer.mid(DATA_META_LEN,FS.toUInt()));
                }
                else
                {
                    file.write(buffer.mid(DATA_META_LEN));
                }
            }
            else
            {
                box("文件创建失败:"+tmp_file);
            }
        }
        else
        {
            QThread::usleep(10);
            if(file.open(QIODevice::Append)){
                if(LEFT_SIZE.length() <= BUF_SIZE.length() && LEFT_SIZE.toInt() < BUF_SIZE.toInt())
                {
                    file.write(buffer.mid(DATA_META_LEN,LEFT_SIZE.toInt()));
                }
                else
                {
                    file.write(buffer.mid(DATA_META_LEN));
                }
            }else{
                QThread::usleep(1000);
                if(file.open(QIODevice::Append)){
                    if(LEFT_SIZE.length() <= BUF_SIZE.length() && LEFT_SIZE.toInt() < BUF_SIZE.toInt())
                    {
                        file.write(buffer.mid(DATA_META_LEN,LEFT_SIZE.toInt()));
                    }
                    else
                    {
                        file.write(buffer.mid(DATA_META_LEN));
                    }
                }
                else
                {
                    QThread::usleep(1500);
                    if(file.open(QIODevice::Append)){
                        if(LEFT_SIZE.length() <= BUF_SIZE.length() && LEFT_SIZE.toInt() < BUF_SIZE.toInt())
                        {
                            file.write(buffer.mid(DATA_META_LEN,LEFT_SIZE.toInt()));
                        }
                        else
                        {
                            file.write(buffer.mid(DATA_META_LEN));
                        }
                    }else{
                        box("失败×× LEFT_SIZE="+LEFT_SIZE);
                    }
                }
            }
        }

        if(LEFT_SIZE.length() <= BUF_SIZE.length() && LEFT_SIZE.toInt() < BUF_SIZE.toInt())
        {
            QThread::usleep(100);
            change_loading_tip("下载完成");
            QString origine_name = file.fileName().replace(".tmp","");
            file.rename(origine_name);
            QThread::usleep(100);
            hide_loading();
            if(1 == OPEN) //打开文件
            {
                QDesktopServices::openUrl(QUrl::fromLocalFile(origine_name));
            }
            else if(2 == OPEN) //定位文件
            {
                this->localtion_file();
            }
            else
            {}
            file.close();
            buffer.clear();

        }
        else
        {
            QThread::usleep(10);
            change_loading_tip("下载中 ("+QString::number(PCT.toFloat(),'f',2)+"%)");
        }
        /////////////////////////////////////////////////////////////////////
    }
    else
    {}
}

void FileExplorer::readyRead()
{
    QByteArray buffer;

    QDataStream socketStream(cmd_socket);
    socketStream.setVersion(QDataStream::Qt_5_15);
    socketStream.startTransaction();
    socketStream >> buffer;
    cmd_socket->flush();

    if(!socketStream.commitTransaction())
    {
        return;
    }

    QByteArray data = buffer.mid(DATA_META_LEN);
    QByteArray header = buffer.mid(0,buffer.indexOf(";"));

    if("DQN:?" != header.mid(0,5))
    {
        return;
    }

    QString _header = header.mid(5);
    //qDebug() << "_header=" << _header << ",data=" << data;

    QString META = "";
    int     CODE = 0;
    QString MSG  = "";
    int     FD_ID  = 0;

    QStringList headers = _header.split(",");
    for(QString hi : headers)
    {
        QStringList his = hi.split(":");
        if("META" == his[0]) //标识符
        {
            META = his[1];
        }
        else if("CODE" == his[0])
        {
            CODE = his[1].toInt();
        }
        else if("MSG" == his[0])
        {
            MSG = his[1];
        }
        else if("SHOW_EMP" == his[0])
        {
            SHOW_EMP = his[1];
        }
        else if("FD_ID" == his[0])
        {
            FD_ID = his[1].toInt();
        }
        else{}
    }
    if("WELCOME" == META) //登录后的绑定
    {
        //qDebug() << "user->job_number=" << user->job_number;
        sendMsgPack("BIND_USER:?S=CMD,JOB_NUMBER="+user->job_number,"",cmd_socket);
        sendMsgPack("BIND_USER:?JOB_NUMBER="+user->job_number,"",download_socket);
    }
    else if("INTENT_RESULT" == META) //创建结果
    {
        if(0 == CODE)
        {
            dlg_create->setVisible(false);
            dlg_create->clear();

            Toast::succ(MSG);
            load_files();
        }
        else
        {
            dlg_create->reset();
            Toast::err(MSG);
        }
    }
    else if("LIST_FILE_RESULT" == META) //列出文件
    {
        this->list_file(data);
    }
    else if("SYNC_UP_STATE" == META || "UPLOAD_SUCCESS" == META)
    {
        QString MD5="";
        unsigned long long LEFT_SIZE = 0.0f;
        float PCT = 0.0f;
        QString BUNDLE = "";
        QString BUNDLE_ID = "0";
        QString FD_ID = "0";

        //同步上传文件进度
        QStringList tokens = _header.split(",");
        for(QString token : tokens)
        {
            QStringList T = token.split(":");
            if("MD5" == T[0]) //标识符
            {
                MD5 = T[1];
            }
            else if("BUNDLE" == T[0]) //部门还是群
            {
                BUNDLE = T[1];
            }
            else if("BUNDLE_ID" == T[0]) //部门或者群ID
            {
                BUNDLE_ID = T[1];
            }
            else if("FD_ID" == T[0]) //哪个文件夹
            {
                FD_ID = T[1];
            }
            else if("LEFT_SIZE" == T[0])
            {
                LEFT_SIZE = T[1].toULongLong();
            }
            else if("PCT" == T[0])
            {
                PCT = T[1].toFloat();
            }
            else{}
        }
        emit sync_file_progrrss(BUNDLE,BUNDLE_ID,FD_ID,MD5,META,PCT);
    }
    else if("OPEN_FILE" == META)
    {
        if(0 != CODE)
        {
            box(MSG);
        }
        else
        {
            QString MD5 = "";
            QString NAME = "";
            QString FILE_ID = "";
            QString OPEN = "0";
            int SIZE = 0;

            for(QString hi : headers)
            {
                QStringList his = hi.split(":");
                if("MD5" == his[0])
                {
                    MD5 = his[1];
                }
                else if("NAME" == his[0])
                {
                    NAME = his[1];
                }
                else if("FILE_ID" == his[0])
                {
                    FILE_ID = his[1].toInt();
                }
                else if("SIZE" == his[0])
                {
                    SIZE = his[1].toInt();
                }
                else if("OPEN" == his[0])
                {
                    OPEN = his[1];
                }
                else{}
            }
            //打开文件
            QString FILE_SRC = USR_CACHE_DIR+"\\"+QString::number(selected_fd->id)+"\\"+MD5;
            QDir FILE_DIR(FILE_SRC);
            if(!FILE_DIR.exists())
            {
                FILE_DIR.mkpath(FILE_SRC);
            }
            QFile _file(FILE_SRC+"\\"+selected_fd->name);
            if(!_file.exists())
            {
                sendMsgPack("DOWN_FILE:?FILE_ID="+QString::number(selected_fd->id)+",META_KEY="+meta->key+",META_ID="+QString::number(meta->id)+",OPEN=1","",download_socket);
            }
            else
            {
                QThread::usleep(10);
                hide_loading();
                //需要判断文件的size是不是正确
                if(SIZE != _file.size())
                {
                    _file.remove();
                    QThread::usleep(10);
                    sendMsgPack("DOWN_FILE:?FILE_ID="+QString::number(selected_fd->id)+",META_KEY="+meta->key+",META_ID="+QString::number(meta->id)+",OPEN=1","",download_socket);
                }
                else
                {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(FILE_SRC+"\\"+selected_fd->name));
                }
            }
        }
    }
    else if("FILE_INFO" == META)
    {
        QString MD5 = "";
        QString FILE_ID = "";
        QString NAME = "";
        QString SIZE = "";
        QString SUFFIX = "";
        QString CREATED_AT = "0";
        QString UPDATED_AT = "0";
        QString VERSION = "0";
        QString CREATOR = "";

        for(QString hi : headers)
        {
            QStringList his = hi.split(":");
            if("MD5" == his[0])
            {
                MD5 = his[1];
            }
            else if("FILE_ID" == his[0])
            {
                FILE_ID = his[1];
            }
            else if("NAME" == his[0])
            {
                NAME = his[1];
            }
            else if("SIZE" == his[0])
            {
                SIZE = his[1];
            }
            else if("SUFFIX" == his[0])
            {
                SUFFIX = his[1];
            }
            else if("CREATED_AT" == his[0])
            {
                CREATED_AT = his[1];
            }
            else if("UPDATED_AT" == his[0])
            {
                UPDATED_AT = his[1];
            }
            else if("VERSION" == his[0])
            {
                VERSION = his[1];
            }
            else if("CREATOR" == his[0])
            {
                CREATOR = his[1];
            }
            else{}
        }

        active_fd->id = FILE_ID.toInt();
        active_fd->name = NAME;
        active_fd->md5 = MD5;
        active_fd->size = SIZE.toULongLong();
        active_fd->suffix = SUFFIX;
        active_fd->created_at = CREATED_AT.toInt();
        active_fd->updated_at = UPDATED_AT.toInt();
        active_fd->version = VERSION;
        active_fd->creator = CREATOR;

        render_active_file_info();
    }
    else if("DOWNLOAD" == META)
    {
        //接收文件 DOWNLOAD:?FILE_ID:%1,MD5:%2,LEFT_SIZE:%3,BUF_SIZE:%4,FS:%5,PCT:%6;
        /////////////////////////////////////////////////////////////////////
        QString FILE_ID ="";
        QString MD5 = "";
        QString LEFT_SIZE = "";
        QString BUF_SIZE = "";
        QString FS = "";
        QString PCT = "";
        QString NAME = "";
        int OPEN = 0;

        QStringList headers = _header.split(",");
        for(int i=0;i<headers.length();i++)
        {
            QString hi = headers[i];
            QStringList his = hi.split(":");
            if("MD5" == his[0])
            {
                MD5 = his[1];
            }
            else if("FILE_ID" == his[0])
            {
                FILE_ID = his[1];
            }
            else if("LEFT_SIZE" == his[0])
            {
                LEFT_SIZE = his[1];
            }
            else if("BUF_SIZE" == his[0])
            {
                BUF_SIZE = his[1];
            }
            else if("FS" == his[0])
            {
                FS = his[1].toLower();
            }
            else if("PCT" == his[0])
            {
                PCT = his[1];
            }
            else if("NAME" == his[0])
            {
                NAME = his[1];
            }
            else if("OPEN" == his[0])
            {
                OPEN = his[1].toInt();
            }
            else{}
        }
        if("0" == FILE_ID)
        {
            return;
        }
        if(LEFT_SIZE.length() > 0 && LEFT_SIZE.toInt() > 0)
        {
            //do this code
        }
        else
        {
            return;
        }
        QString LOCAL_CACHE_DIR = get_reg("LOCAL_CACHE_DIR");

        QString save_path(LOCAL_CACHE_DIR+"\\"+user->job_number+"\\"+FILE_ID+"\\"+MD5);
        QDir save_dir(save_path);
        if(!save_dir.exists())
        {
            save_dir.mkpath(save_path);
        }

        QString f = save_path+"\\"+NAME;

        QFile file(f);

        if(!file.exists())
        {
            if(file.open(QIODevice::WriteOnly)){
                if(FS.length() <=6 && FS.toUInt() <= BUF_SIZE.toULongLong())
                {
                    file.write(buffer.mid(DATA_META_LEN,FS.toUInt()));
                }
                else
                {
                    file.write(buffer.mid(DATA_META_LEN));
                }
            }
            else
            {
                //////qDebug() << "文件创建失败:" << f;
            }
        }
        else
        {
            if(file.open(QIODevice::Append)){
                if(LEFT_SIZE.length() <= BUF_SIZE.length() && LEFT_SIZE.toInt() < BUF_SIZE.toInt())
                {
                    file.write(buffer.mid(DATA_META_LEN,LEFT_SIZE.toInt()));
                }
                else
                {
                    file.write(buffer.mid(DATA_META_LEN));
                }
            }else{
                QThread::usleep(1000);
                if(file.open(QIODevice::Append)){
                    if(LEFT_SIZE.length() <= BUF_SIZE.length() && LEFT_SIZE.toInt() < BUF_SIZE.toInt())
                    {
                        file.write(buffer.mid(DATA_META_LEN,LEFT_SIZE.toInt()));
                    }
                    else
                    {
                        file.write(buffer.mid(DATA_META_LEN));
                    }
                }
                else
                {
                    QThread::usleep(1500);
                    if(file.open(QIODevice::Append)){
                        if(LEFT_SIZE.length() <= BUF_SIZE.length() && LEFT_SIZE.toInt() < BUF_SIZE.toInt())
                        {
                            file.write(buffer.mid(DATA_META_LEN,LEFT_SIZE.toInt()));
                        }
                        else
                        {
                            file.write(buffer.mid(DATA_META_LEN));
                        }
                    }else{
                        //////qDebug() << "失败×× LEFT_SIZE=" << LEFT_SIZE;
                    }
                }
            }
        }

        if(LEFT_SIZE.length() <= BUF_SIZE.length() && LEFT_SIZE.toInt() < BUF_SIZE.toInt())
        {
            QThread::sleep(1);
            if(1 == OPEN)
            {
                //////qDebug() << "接收完成,直接打开";
                QDesktopServices::openUrl(QUrl::fromLocalFile(f));
            }
            else
            {
                //////qDebug() << "接收完成";
            }
            file.close();
            buffer.clear();
        }
        else
        {
            QThread::usleep(50);
            ////////qDebug() << "BUF_SIZE=" << BUF_SIZE << ",LEFT_SIZE=" << LEFT_SIZE<< ",PCT=" << PCT;
        }
        /////////////////////////////////////////////////////////////////////
    }
    else if("BINDED" == META)
    {
        QJsonParseError err_rpt;
        QJsonDocument  doc = QJsonDocument::fromJson(data, &err_rpt);
        if(err_rpt.error == QJsonParseError::NoError)
        {
            QJsonArray arr = doc.array();
            QJsonObject obj = arr.at(0).toObject();
            QString views = obj["views"].toString();
            emit sync_views(views);

            QString ps = obj["permissions"].toString();
            QJsonDocument  psd = QJsonDocument::fromJson(ps.toUtf8());
            QJsonArray permission_arr = psd.array();

            for (int i=0; i<permission_arr.count(); i++)
            {
                QJsonObject po = permission_arr.at(i).toObject();

                QString bundle = po.value("bundle").toString();
                int bundle_id  = po.value("bundle_id").toInt();
                QString pname  = po.value("pname").toString();
                int pad        = po.value("pad").toInt();

                permission["P_"+bundle+"_"+QString::number(bundle_id)+"_"+pname] = pad;
            }
        }else{
            qDebug() << "绑定解析错误";
        }
    }
    else if("DEP_EMPLOYEES" == META)
    {
        if("SHOW" == SHOW_EMP)
        {
            this->render_employees(data);
        }
        this->flush(this->width(),this->height());
    }
    else if("DELETE_FILE" == META)
    {
        //删除文件的结果
        if(0 == CODE)
        {
            Toast::succ(MSG);
            load_files();
        }
        else
        {
            MSGBOX::alert(parentWidget(),MSG);
        }
    }
    else if("DIRMAP" == META)
    {
        this->dir_map(_header,data);
    }
    else if("ERROR" == META)
    {
        mask->hide();
        dir_process->hide();
        MSGBOX::error(this,MSG);
    }
    else
    {
        ////qDebug() << "### 未知的META ###" << META;
    }
}

void FileExplorer::render_active_file_info()
{
    QString ico = suffix2icon(active_fd->suffix);
    active_icon->setPixmap(QPixmap::fromImage(QImage(ico)));
    active_title->setText(active_fd->name);

    render_attribute();
}

QString FileExplorer::suffix2icon(QString suffix)
{
    if(typesRes.indexOf(suffix.toLower()) == -1)
    {
        return ":/Resources/types/null.png";
    }
    return ":/Resources/types/"+suffix.toLower()+".png";
}

void FileExplorer::render_base_info()
{
    //文件基本信息
    base_info = new QWidget(ListSide);
    base_info->move(0,0);
    base_info->resize(side_tab->width(),side_tab->height());
    base_info->setObjectName("base_info");
    base_info->setStyleSheet("#base_info{background:#fff}");

    //添加TAB
    side_tab->addTab(base_info,"基本信息");

    //吸底动作栏
    file_min_action = new QWidget(ListSide);
    file_min_action->setObjectName("file_min_action");
    file_min_action->move(10,ListSide->height()-(SIDE_MIN_ACTIONS_BAR_HEIGHT+10));
    file_min_action->resize(LIST_SIDE_WIDTH1-20,SIDE_MIN_ACTIONS_BAR_HEIGHT);
    file_min_action->setStyleSheet("#file_min_action{background:#313136;border-radius:4px;}");

    //文件属性
    QVector<ATTRIBUTE*> attributes;

    ATTRIBUTE* space_0 = new ATTRIBUTE();
    space_0->key = "space_0";
    space_0->name = "";
    space_0->value = "";

    ATTRIBUTE* version = new ATTRIBUTE();
    version->key = "version";
    version->name = "版本号:";

    ATTRIBUTE* extraction_cod = new ATTRIBUTE();
    extraction_cod->key = "extraction_cod";
    extraction_cod->name = "提取码:";

    ATTRIBUTE* file_size = new ATTRIBUTE();
    file_size->key = "file_size";
    file_size->name = "文件大小:";

    ATTRIBUTE* create_time = new ATTRIBUTE();
    create_time->key = "created_time";
    create_time->name = "创建时间:";

    ATTRIBUTE* update_time = new ATTRIBUTE();
    update_time->key = "updated_time";
    update_time->name = "更新时间:";

    ATTRIBUTE* creator = new ATTRIBUTE();
    creator->key = "creator";
    creator->name = "创建人:";

    ATTRIBUTE* md5 = new ATTRIBUTE();
    md5->key = "md5";
    md5->name = "校验码:";

    attributes.append(space_0);
    attributes.append(version);
    attributes.append(extraction_cod);
    attributes.append(file_size);
    attributes.append(create_time);
    attributes.append(update_time);
    attributes.append(creator);
    attributes.append(md5);

    for(int i=0;i<attributes.count();i++)
    {
        QLabel* attr_label = new QLabel(base_info);
        attr_label->setText(attributes[i]->name);
        attr_label->setObjectName("attr_label_"+attributes[i]->key);
        attr_label->setStyleSheet("#attr_label_"+attributes[i]->key+"{font-size:14px;background:#fff;}");
        attr_label->resize(100,35);
        attr_label->move(0,i*40);
        attr_label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

        QLabel* attr_value = new QLabel(base_info);
        attr_value->setText(attributes[i]->value);
        attr_value->setObjectName("attr_"+attributes[i]->key);
        attr_value->setStyleSheet("#attr_"+attributes[i]->key+"{font-size:14px;background:#fff;}");
        attr_value->resize(base_info->width() - 100,35);
        attr_value->move(115,i*40);
        attr_value->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    }
}

void FileExplorer::render_attribute()
{
    QLabel* extraction_cod = findChild<QLabel*>("attr_extraction_cod");
    if(extraction_cod)
    {
        extraction_cod->setText(QString::number(active_fd->id));
    }

    QLabel* file_size = findChild<QLabel*>("attr_file_size");
    if(file_size)
    {
        file_size->setText(ConverSize(active_fd->size));
    }

    QLabel* md5 = findChild<QLabel*>("attr_md5");
    if(md5)
    {
        md5->setText(active_fd->md5.mid(0,8).toUpper());
    }

    QLabel* version = findChild<QLabel*>("attr_version");
    if(version)
    {
        version->setText(active_fd->version);
    }

    QLabel* created_time = findChild<QLabel*>("attr_created_time");
    if(created_time)
    {
        QDateTime dt1;
        dt1.setTime_t((unsigned int)active_fd->created_at);
        created_time->setText(dt1.toString("yyyy-MM-dd hh:mm:ss"));
    }

    QLabel* updated_time = findChild<QLabel*>("attr_updated_time");
    if(updated_time)
    {
        QDateTime dt2;
        dt2.setTime_t((unsigned int)active_fd->updated_at);
        updated_time->setText(dt2.toString("yyyy-MM-dd hh:mm:ss"));
    }

    QLabel* creator = findChild<QLabel*>("attr_creator");
    if(creator)
    {
        creator->setText(active_fd->creator);
    }
}

QString FileExplorer::ConverSize(unsigned long long bytes)
{
    float size = bytes;
    QList<QString> suffix = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    int total = suffix.count();
    int i = 0;
    for (; size > 1024 && i < total; i++)
    {
        size /= 1024;
    }
    return QString::number(size,'f',2) +" "+ suffix[i];
}

//文件有变动
void FileExplorer::FileChanged(const QString& file_path)
{
    if(file_path.length() < 10)
    {
        return;
    }
    if(!file_path.contains(user->job_number))
    {
        return;
    }
    int pos = file_path.indexOf(user->job_number);
    QString info = file_path.mid(pos+user->job_number.length()+1);
    QStringList infos = info.split("\\");
    if(infos.length() != 3)
    {
        return;
    }
    //qDebug() << "FileExplorer::FileChanged...." << file_path;
    QFileInfo fi(file_path);

    QString file_id = infos[0];
    QString file_md5 = md5_file(file_path);
    QString file_name = infos[2];

    //自己的文件
    //群组的公共文件
    //其他人的文件
    if("MY_FILE" == this->meta->key)
    {
        //do nothing
    }
    else
    {
        if(SJN.length() == 0)
        {
            //群组的公共文件
            if(!has_permission("EDIT_FILE"))
            {
                return;
            }
        }
        else
        {
            if(user->job_number == SJN)
            {
                //自己
            }
            else
            {
                if(!has_permission("MANAGE_EMPLOYEE"))
                {
                    return;
                }
            }
        }
    }

    UP_FILE* upload_file = new UP_FILE();
    upload_file->id = file_id.toInt();
    upload_file->suffix = fi.suffix();
    upload_file->ico = suffix2icon(fi.suffix());
    upload_file->md5 = file_md5;
    upload_file->name = file_name;
    upload_file->path = file_path;
    upload_file->size = fi.size();
    upload_file->state = UP_STATE::UPDATE;

    //要不要清除
    upload_pannel->clear_queue();
    wait(5);
    upload_pannel->add_queue(upload_file);
    wait(5);
    upload_pannel->touch_upload(this->meta->key,this->meta->id,fd->id,SJN);
}

//初始化版本
void FileExplorer::render_version_list()
{
    version_list = new QWidget(ListSide);
    version_list->move(0,0);
    version_list->resize(side_tab->width(),side_tab->height());
    version_list->setObjectName("version_list");
    version_list->setStyleSheet("#version_list{background:#fff}");

    //添加TAB
    side_tab->addTab(version_list,"修改记录");
}

//打开文件夹和文件
void FileExplorer::fd_open()
{
    if(NULL != selected_fd)
    {
        //判断是文件还是文件夹
        if("FOLDER" == selected_fd->suffix)
        {
            fd = selected_fd;
            load_files();

            //如果是文件夹 需要刷新URLBAR
            emit append_urlbar(fd);
        }
        else
        {
            //如果没有打开文件的权限，双击也不能打开
            if(!has_permission("OPEN_FILE"))
            {
                return;
            }
            //需要去文件服务器查询最新的文件md5,因为有可能在其他电脑上更新了文件
            sendMsgPack("OPEN_FILE:?FILE_ID="+QString::number(selected_fd->id)+",OPEN:1,META_KEY="+meta->key+",META_ID="+QString::number(meta->id),"",cmd_socket);
        }
    }else{
        box("无法打开文件，因为文件句柄为空");
    }
}

void FileExplorer::row_item_section_changed()
{
    //qDebug() << "row_item_section_changed...." << listWapper->selectedItems();
}

//发送socket消息包
bool FileExplorer::sendMsgPack(QString HEADER, QString MSG,QTcpSocket* socket)
{
    if(!socket)
    {
        return false;
    }
    if(!socket->isOpen())
    {
        return false;
    }

    if(";" != HEADER[HEADER.length()-1])
    {
        HEADER += ";";
    }

    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_15);

    QByteArray header;
    header.prepend(QString(HEADER).toUtf8());
    header.resize(DATA_META_LEN);

    if(MSG.length() > 0)
    {
        header.append(QString(MSG).toUtf8());
    }

    socketStream << header;

    return true;
}

//和socket服务器断开连接
void FileExplorer::disconnected()
{
    cmd_socket->abort();
    cmd_socket->deleteLater();

    box("与文件服务器断开了连接..."); //需要考虑自动重新连接服务器
}

//渲染列表头
void FileExplorer::render_list_header()
{
    file_row_header = new QWidget(canvas);
    file_row_header->setObjectName("file_row_header");
    file_row_header->setStyleSheet("#file_row_header{background:#F8F9F9;border-bottom:1px solid #E7E7E7;color:#000;}");
    file_row_header->resize(this->width() - LIST_SIDE_WIDTH1 - emp_width,35);
    file_row_header->move(0,0);

    Label* row_header_ico = new Label(file_row_header);
    row_header_ico->resize(50,35);
    row_header_ico->move(0,0);

    row_header_name = new Label(file_row_header);
    row_header_name->setObjectName("row_header_name");
    row_header_name->resize(475,35);
    row_header_name->move(55,0);
    row_header_name->setText("名称");
    row_header_name->setStyleSheet("#row_header_name{font-weight:bold;}");

    row_header_user = new Label(file_row_header);
    row_header_user->setObjectName("row_header_user");
    row_header_user->resize(180,35);
    row_header_user->move(row_header_name->x() + row_header_name->width() + 10,0);
    row_header_user->setText("创建人");
    row_header_user->setStyleSheet("#row_header_user{font-weight:bold;}");

    row_header_version = new Label(file_row_header);
    row_header_version->setObjectName("row_header_version");
    row_header_version->resize(80,35);
    row_header_version->move(row_header_user->x() + row_header_user->width(),0);
    row_header_version->setText("版本");
    row_header_version->setAlignment(Qt::AlignCenter);
    row_header_version->setStyleSheet("#row_header_version{font-weight:bold;}");

    row_header_size = new Label(file_row_header);
    row_header_size->setObjectName("row_header_size");
    row_header_size->resize(90,35);
    row_header_size->move(row_header_version->x() + row_header_version->width(),0);
    row_header_size->setText("文件尺寸");
    row_header_size->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    row_header_size->setStyleSheet("#row_header_size{font-weight:bold;}");
}

//显示loading
void FileExplorer::show_loading(QString tip)
{
    QTimer::singleShot(2, this, [=](){
        if(!in_loading)
        {
            QMovie *movie = new QMovie(":/Resources/load.gif");
            load->setMovie(movie);
            movie->start();

            loading_tip->setText(tip);

            in_loading = true;
            mask->show();
            load->show();
        }
    });
}

//隐藏loading
void FileExplorer::hide_loading()
{
    QTimer::singleShot(10, this, [=](){
        if(in_loading)
        {
            in_loading = false;
            mask->hide();
            load->hide();
        }
    });
}

void FileExplorer::change_loading_tip(QString tip)
{
    loading_tip->setText(tip);
}

//来自那个系列
void FileExplorer::set_meta(UrlMeta *_meta)
{
    fd_menu->hide();
    selected_fd->id = 0;
    this->meta = _meta;
    dlg_create->setMeta(_meta);
    emplyees->clearSelection();
    SJN = "";
    if("DEP"==meta->key)
    {
        reload_album->setText("部门数据" );
    }
    else if("GROUP"==meta->key)
    {
        reload_album->setText("群组数据" );
    }
    else
    {}

    //    QMap<QString, int>::const_iterator i = permission.constBegin();
    //    while (i != permission.constEnd()) {
    //        qDebug() << i.key() << ": " << i.value();
    //        ++i;
    //    }

    //    qDebug() << "META:: key=" << this->meta->key << ",id=" << this->meta->id;

    //创建文件夹权限
    if(has_permission("CREATE_FOLDER"))
    {
        emit sync_permission("create",1);
    }
    else
    {
        emit sync_permission("create",0);
    }
    //上传文件权限
    if(has_permission("UPLOAD_FILE"))
    {
        emit sync_permission("upload",1);
    }
    else
    {
        emit sync_permission("upload",0);
    }

    //载入文件
    load_files();
}

//更换目录
void FileExplorer::change_folder(UrlMeta* _meta)
{
    fd->id = _meta->id;
    load_files();
}

//1.发起文件请求
void FileExplorer::load_files()
{
    this->clear();
    show_loading();
    QString cmd = "LIST_FILES:?FD_ID="+QString::number(fd->id)+",META_KEY="+meta->key+",META_ID="+QString::number(meta->id)+",JOB_NUMBER="+user->job_number+",SJN="+SJN;
    sendMsgPack(cmd,"",cmd_socket);
}

//2.收到文件数据
void FileExplorer::list_file(QString data)
{
    //qDebug() << "list_file=" << data;
    QJsonParseError err_rpt;
    QJsonDocument  jsonDoc = QJsonDocument::fromJson(data.toUtf8(), &err_rpt);
    QJsonArray arr =  jsonDoc.array();
    for (int i=0; i<arr.count(); i++) {
        QJsonObject fdo = arr[i].toObject();
        FD* fd = new FD();
        fd->id = fdo.value("id").toInt();
        fd->md5 = fdo.value("md5").toString();
        fd->name = fdo.value("name").toString();
        fd->suffix = fdo.value("suffix").toString();
        fd->size = fdo.value("size").toString().toULongLong();
        fd->created_at = fdo.value("created_at").toString().toInt();
        fd->updated_at = fdo.value("updated_at").toString().toInt();
        fd->version = fdo.value("version").toString();
        fd->creator = fdo.value("creator").toString();
        QString ico_res = ":/Resources/types/"+fd->suffix.toLower()+".png";
        if(typesRes.indexOf(fd->suffix.toLower()) == -1)
        {
            ico_res = ":/Resources/types/null.png";
        }
        fd->icon = ico_res;

        fds.append(fd);
    }
    //渲染文件
    render_files();

}

//列表形式 单击行
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

            QThread::usleep(10);
            sendMsgPack("FILE_INFO:?FILE_ID="+QString::number(selected_fd->id)+",META_KEY="+meta->key+",META_ID="+QString::number(meta->id)+"","",cmd_socket);
        }
    }
}

//列表形式 双击行
void FileExplorer::row_db_clicked(QListWidgetItem *item)
{
    db_click = true;
    fd_open();
}

void FileExplorer::Refresh()
{
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
        box("UnKnow DisplayMod:"+DisplayMod);
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
            row->resize(this->width());

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

                //////////////////////////
                /// 权限判断
                //////////////////////////
                if("FOLDER" == fds[i]->suffix)
                {
                    fd_menu->set_enable("open",true);
                    fd_menu->set_enable("location",false);

                    //重命名
                    fd_menu->set_enable("rename",has_permission("RENAME_FOLDER"));

                    //删除
                    fd_menu->set_enable("delete",has_permission("DELETE_FOLDER"));
                }
                else
                {
                    fd_menu->set_enable("open",has_permission("OPEN_FILE"));
                    fd_menu->set_enable("location",has_permission("OPEN_FILE"));

                    //重命名
                    fd_menu->set_enable("rename",has_permission("RENAME_FILE"));

                    //删除
                    fd_menu->set_enable("delete",has_permission("DELETE_FILE"));
                }
                //移动
                fd_menu->set_enable("move",has_permission("MOVE_FILE"));

                wait(5);
                //开始判断 其他人的话，是不是有管理权限
                if("MY_FILE" != this->meta->key && SJN.length() > 0 && user->job_number != SJN)
                {
                    fd_menu->set_enable("move",has_permission("MANAGE_EMPLOYEE"));
                    fd_menu->set_enable("rename",has_permission("MANAGE_EMPLOYEE"));
                    fd_menu->set_enable("delete",has_permission("MANAGE_EMPLOYEE"));
                }

                fd_menu->set_suffix(fds[i]->suffix);
                fd_menu->move(row->x()+pos.x()+emplyees->width(), row->y()+pos.y()+35);
                fd_menu->show();
                fd_menu->raise();

                QThread::usleep(10);
                sendMsgPack("FILE_INFO:?FILE_ID="+QString::number(selected_fd->id)+",META_KEY="+meta->key+",META_ID="+QString::number(meta->id)+"","",cmd_socket);
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
            QString name = fds[i]->name;
            QString type = fds[i]->suffix;

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
                //////qDebug() << "UnKnow File type:" << type.toLower();
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

            QDateTime dt;
            dt.setTime_t(fds[i]->created_at);

            Label* fd_date = new Label(fd_item);
            fd_date->setText(dt.toString("yyyy-MM-dd hh:mm:ss"));
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
    ////////qDebug() << "移动 x=" << event->pos().x() << ",y=" << event->pos().y();
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
    listWapper->clear();
}

//单击文件
void FileExplorer::fd_clicked(QMouseEvent *event, int id)
{
    fd_menu->hide();
    if(Qt::LeftButton == event->button())
    {
        //////qDebug() << "event->type()=" << event->type();
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
    //////qDebug() << "单击或者双击，选中文件,selected_fd=" << selected_fd->name;
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

//打开上传下拉
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

        if(str_path_list.size() > MAX_UPLOAD_FILES)
        {
            box("一次最多只能选择"+ QString::number(MAX_UPLOAD_FILES) +"个文件!你选择了:"+QString::number(str_path_list.size())+"个");
            return;
        }

        for (int i = 0; i < str_path_list.size(); i++){
            QString abs_path = str_path_list[i]; //包含文件名称的绝对路径
            this->moveto_queue(abs_path);
        }
        upload_pannel->show();
        upload_pannel->raise();
        upload_pannel->touch_upload(this->meta->key,this->meta->id,fd->id,SJN);
    }
    else if("folder" == IntentType) //上传文件夹
    {
        upload_dir = QFileDialog::getExistingDirectory(this, "请选择目录", "D:\\");

        if (upload_dir.isEmpty())
        {
            return;
        }
        else
        {
            file_queue.clear();
            mask->show();
            dir_process->init(fd->id);
            dir_process->show();
            dir_process->raise();
            dir_process->clear_cache_data();

            dir_process->set_data("dir_name",upload_dir);
            dir_process->set_data("dir_path",upload_dir);

            //开启一个线程去去处理文件数量和尺寸
            ProcessDirThread* t = new ProcessDirThread();
            t->set_dir(upload_dir);
            t->start();

            total_size = 0;
            file_queue.clear();
            for(int i=0; i<dirArr.count(); i++) {
                dirArr.removeAt(0);
            }

            connect(t,&ProcessDirThread::find_file,this,[=](QString T, QString file_path){
                if("DIR" == T)
                {
                    dir_process->set_data("ADD_DIR","");

                    dirArr.append({file_path});
                }
                else if("FILE" == T)
                {
                    dir_process->set_data("ADD_FILE","");
                    file_queue.append(file_path.replace(upload_dir,"$0"));
                }
                else
                {}
            });
            connect(t,&ProcessDirThread::sync_size,this,[=](unsigned long long size){
                total_size += size;
                dir_process->set_data("ADD_SIZE",QString::number(size));
            });

            connect(t,&ProcessDirThread::process_complete,dir_process,[=](){
                dir_process->changeto_prepareing();

                //牵扯到的子文件夹
                QString data = QJsonDocument(dirArr).toJson(QJsonDocument::Compact).constData();

                //发给服务器，服务器批量创建好子文件夹后，返回子文件夹和ID的映射后，开始在回调中开始上传
                sendMsgPack("CREATE_DIRS:?FILE_ID="+QString::number(fd->id)+",META_KEY="+meta->key+",BASE_DIR="+upload_dir+",SJN="+SJN+",META_ID="+QString::number(meta->id),data,cmd_socket);
            });
        }
    }
    else if("drag" == IntentType)
    {
        //qDebug() << "拖动上传";
    }
    else
    {
        //do nothing....
    }
}

//开始上传文件
void FileExplorer::dir_map(QString _header, QString data)
{
    _header.replace(":/","$#$");
    dir_process->changeto_uploading(total_size);qDebug() << "total_size=" << total_size;

    QStringList headers = _header.split(",");
    for(QString hi : headers)
    {
        QStringList his = hi.split(":");
        if("BASE_DIR" == his[0])
        {
            BASE_DIR = his[1];
            BASE_DIR.replace("$#$",":/");
        }
        else if("BASE_ID" == his[0])
        {
            BASE_ID = his[1].toInt();
        }
        else{}
    }

    //渲染目录ID映射
    QJsonParseError err_rpt;
    QJsonDocument  jsonDoc = QJsonDocument::fromJson(data.toUtf8(), &err_rpt);
    QJsonArray _map =  jsonDoc.array();
    for (int i=0; i< _map.count(); i++) {
        QJsonObject map = _map[i].toObject();

        QString path = map.value("path").toString();
        int     id   = map.value("id").toInt();

        DirIdMap[path] = id;
    }
    QThread::usleep(5);
    this->touch_upload();
}

//将要上传的文件压入上传面板的队列中
void FileExplorer::moveto_queue(QString abs_path)
{
    UP_FILE* upload_file = this->get_file_handle(abs_path);
    wait(5);
    upload_pannel->add_queue(upload_file);
}

UP_FILE *FileExplorer::get_file_handle(QString abs_path)
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

    QString _md5 = "";

    //只要是office的文件 都直接用文件的md5值来计算
    if(office.indexOf(upload_file->suffix) > 0)
    {
        _md5 = md5_file(abs_path);
    }
    else
    {
        _md5 = md5(abs_path + user->job_number + QString::number(fize_size) + QString::number(get_time()) + random(6));
    }

    upload_file->md5 = _md5;
    upload_file->name = file_name;
    upload_file->path = abs_path;
    upload_file->size = fize_size;
    upload_file->state = UP_STATE::UPLOAD;

    return upload_file;
}

void FileExplorer::touch_upload()
{
    if(!uploading && file_queue.count() > 0)
    {
        uploading = true;

        QString fork_path = file_queue[0];
        QString abs_file = fork_path;
        abs_file.replace("$0",BASE_DIR);
        int file_dir_id = DirIdMap[fork_path.mid(0,fork_path.lastIndexOf("/"))];

        UP_FILE* F = this->get_file_handle(abs_file);
        dir_process->set_data("SET_CURR_FILE",abs_file);

        FileManager* fm = new FileManager();
        fm->set_socket_descriptor(this->upload_socket_descriptor);
        fm->set_file(this->meta->key,this->meta->id,file_dir_id,SJN,F);
        fm->start();

        connect(fm,&FileManager::update_progress,this,&FileExplorer::sync_dir_upload);
    }
    else
    {
        mask->hide();
        dir_process->hide();
        dir_process->set_data("COMPLETE","");
        Refresh();
        MSGBOX::alert(this,"全部上传完成");
        dir_process->clear_cache_data();
    }
}

void FileExplorer::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "dragEnterEvent";
}

void FileExplorer::dropEvent(QDropEvent *event)
{
    qDebug() << "dropEvent";
}

void FileExplorer::sync_dir_upload(QString BUNDLE,QString BUNDLE_ID,QString FD_ID,QString md5, QString state, float pct,unsigned long long SPEED,int buf_size)
{
    dir_process->update_upload_progress(buf_size);
    dir_process->set_data("UPDATE_PCT",QString::number(pct));
    dir_process->set_data("SPEED",QString::number(SPEED));
    if("UPLOAD_COMPLETE" == state && 100 == pct)
    {
        dir_process->set_data("FILE+1","");
        if(file_queue.count() > 0)
        {
            file_queue.removeFirst();
            uploading = false;
            QThread::usleep(10);
            this->touch_upload();
        }
    }
}

//点击了右键菜单
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
    fd_menu->hide();
    if("open" == key) //打开文件
    {
        fd_open();
    }
    else if("location" == key) //定位文件
    {
        this->localtion_file();
    }
    else if("sendto_ding_employee" == key) //发送给钉钉同事
    {
        //
    }
    else if("share" == key) //共享
    {
        //
    }
    else if("move" == key) //移动
    {
        //
    }
    else if("rename" == key) //重命名
    {
        //
    }
    else if("delete" == key) //删除
    {
        if(ChosseResult::ID_OK == MSGBOX::question(parentWidget(),"确定删除 "+selected_fd->name+" 吗?"))
        {
            //确定删除
            sendMsgPack("DELETE_FILE:?FILE_ID="+QString::number(selected_fd->id)+",CONFIRM_JOB_NUMBER="+user->job_number,"",cmd_socket);
        }
    }
    else if("attribute" == key) //属性
    {
        //
    }
    else
    {
        box("点击了未知的菜单:"+key);
    }
}

//打开定位文件
void FileExplorer::localtion_file()
{
    QProcess process;
    if(selected_fd->md5.length() < 16)
    {
        MSGBOX::alert(this,"MD5值丢失，无法定位文件");
        return;
    }
    QString filePath = get_reg("LOCAL_CACHE_DIR") + "\\" + user->job_number + "\\" + QString::number(selected_fd->id) + "\\" + selected_fd->md5 + "\\" + selected_fd->name;
    if("FOLDER" != selected_fd->suffix)
    {
        QFile _file(filePath);
        if(!_file.exists())
        {
            //文件不存在 提示要不要下载
            if(BUTTON_OK == MSGBOX::question(parentWidget(),"文件不存在,是否下载?"))
            {
                show_loading();
                sendMsgPack("DOWN_FILE:?FILE_ID="+QString::number(selected_fd->id)+",META_KEY="+meta->key+",META_ID="+QString::number(meta->id)+",OPEN=2","",download_socket);
            }
        }
        else
        {
            //本地文件存在
            QString cmd = QString("explorer.exe /select,\"%1\"").arg(filePath);
            process.startDetached(cmd);
            QThread::usleep(10);
            hide_loading();
        }
    }
}

//第一次运行
void FileExplorer::OnStart()
{
    emplyees->clear();
    SJN = "";
    if("DEP" == this->meta->key || "GROUP" == this->meta->key)
    {
        //是不是有查看的权限
        sendMsgPack("GET_EMP:?META_KEY="+this->meta->key+",META_ID="+QString::number(this->meta->id)+"","",cmd_socket);
    }
}

//选择了部门员工
void FileExplorer::select_employee(QListWidgetItem *item)
{
    fd_menu->hide();
    SJN = item->data(Qt::UserRole).toString();
    emit url_back_to(meta); //切换了用户，URL地址栏要重置到部门
    this->clear();

    //如果是自己，赋予全部权限，如果不是自己 需要判断是不是可以看文件
    if(user->job_number == SJN)
    {
        emit sync_permission("create",1);
        emit sync_permission("upload",1);
        show_loading();
        sendMsgPack("LIST_FILES:?FD_ID=-1,META_KEY="+meta->key+",META_ID="+QString::number(meta->id)+",SJN="+SJN+",JOB_NUMBER="+user->job_number,",",cmd_socket);
    }
    else
    {
        //是不是可以管理成员文件
        if(has_permission("MANAGE_EMPLOYEE"))
        {
            emit sync_permission("create",1);
            emit sync_permission("upload",1);
        }
        else
        {
            emit sync_permission("create",0);
            emit sync_permission("upload",0);
        }
        //是不是可以看部门成员的文件
        if(has_permission("VIEW_EMP_FILES"))
        {
            show_loading();
            sendMsgPack("LIST_FILES:?FD_ID=-1,META_KEY="+meta->key+",META_ID="+QString::number(meta->id)+",SJN="+SJN+",JOB_NUMBER="+user->job_number,",",cmd_socket);
        }
        else
        {
            Toast::err("暂无权限查看:["+empMap[SJN]->dep_name+"]的"+empMap[SJN]->name);
        }
    }
}

//渲染部门员工列表
void FileExplorer::render_employees(QString data)
{
    empMap.clear();
    QJsonParseError err_rpt;
    QJsonDocument  jsonDoc = QJsonDocument::fromJson(data.toUtf8(), &err_rpt);
    QJsonArray _employees =  jsonDoc.array();
    for (int i=0; i< _employees.count(); i++) {
        QJsonObject employee = _employees[i].toObject();

        QString emp_name   = employee.value("name").toString();
        QString emp_depid = employee.value("dep_id").toString();
        QString emp_depname = employee.value("dep_name").toString();
        QString emp_job_number = employee.value("job_number").toString();

        EMP* emp = new EMP();
        emp->dep_id = emp_depid;
        emp->job_number = emp_job_number;
        emp->dep_name = emp_depname;
        emp->name = emp_name;

        empMap[emp->job_number] = emp;

        EmployeeRow* emp_row = new EmployeeRow();
        emp_row->setObjectName("DEP_"+emp_depid+"_EMP_"+emp_job_number);
        emp_row->set_emp(emp);

        QListWidgetItem* item = new QListWidgetItem(emplyees);
        item->setData(Qt::UserRole,emp->job_number);
        item->setSizeHint(QSize(120,36));

        emplyees->setItemWidget(item, emp_row);
    }
}

bool FileExplorer::has_permission(QString permission_name)
{
    //如果是自己的文件，直接是有权限操作
    if("MY_FILE" == this->meta->key)
    {
        return true;
    }
    if(user->job_number == SJN)
    {
        return true;
    }
    QString key = "P_"+this->meta->key+"_"+QString::number(this->meta->id)+"_"+permission_name;
    if(permission.contains(key))
    {
        if(permission[key] == 1)
        {
            return true;
        }
    }
    return false;
}

FileExplorer::~FileExplorer()
{
}
