#pragma execution_character_set("utf-8")
#include "stackfileMain.h"
#include <QDebug>
#include <math.h>

StackFileMain::StackFileMain(QWidget *parent) : BaseController(parent)
{
    stack_file_welcome = new FileWelcome();
    stack_file_explorer = new FileExplorer();

    connect(stack_file_explorer,SIGNAL(append_urlbar(FD*)),this,SLOT(append_urlbar(FD*)));
    connect(stack_file_explorer,&FileExplorer::sync_views,stack_file_welcome,&FileWelcome::sync_views);
    connect(stack_file_explorer,&FileExplorer::url_back_to,this,&StackFileMain::url_back_to); //重置URL
    //清除员工数据后，载入部门或者群组数据
    connect(stack_file_explorer,SIGNAL(reload_meta_data(UrlMeta*)),this,SLOT(reload_meta_data(UrlMeta*)));
    InitNavigate();
    InitAction();
    InitContent();
    InitUrlBar();

    //开启线程 监听文件检测
    QString USR_DIR = get_reg("LOCAL_CACHE_DIR")+"\\"+user->job_number+"\\";
    watcher_thread = new WatcherThread(USR_DIR);

    connect(watcher_thread,&WatcherThread::changed,stack_file_explorer,&FileExplorer::FileChanged);
    //connect(watcher_thread, SIGNAL(lostChanges()),this, SIGNAL(lostChanges()));
    connect(watcher_thread, &WatcherThread::ready,this, []() { /*qDebug() << "watch thread ready!";*/ });

    watcher_thread->start();
    _timer.start();
}

StackFileMain::~StackFileMain()
{
    watcher_thread->stop();
    watcher_thread->wait();
    delete watcher_thread;
}

void StackFileMain::reload_meta_data(UrlMeta* meta)
{
    this->url_back_to(meta);
    wait(50);
    this->url_meta_clicked(meta);
}

void StackFileMain::appendSubPaths(QDir dir, QStringList& subPaths)
{
    QStringList newSubPaths = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    for (int i = 0; i < newSubPaths.size(); i++)
    {
        QString path = dir.path() + "/" + newSubPaths[i];
        QFileInfo fileInfo(path);
        subPaths.append(path);
        if (fileInfo.isDir())
        {
            QDir dir(path);
            appendSubPaths(dir, subPaths);
        }
    }
}

void StackFileMain::url_back_to(UrlMeta* meta)
{
    fd->id = 0;
    UrlMetas.clear();

    UrlMeta* home = new UrlMeta();
    home->key = "HOME";
    home->text = "首页";
    home->category = "SYSTEM";
    home->id = 0;
    UrlMetas.append(home);
    UrlMetas.append(meta);

    render_urlbar();
}

void StackFileMain::InitNavigate()
{
    this->setStyleSheet("font-family: \"Microsoft Yahei\";");

    navigate = new QWidget(this);
    navigate->setObjectName("navigate");
    navigate->move(0,0);
    navigate->setStyleSheet("#navigate{background:#F3F3F3;border-bottom:1px solid #E5E5E5;}");

    //左箭头
    quick_left = new Label(navigate);
    quick_left->setObjectName("quick_left");
    quick_left->setStyleSheet("#quick_left{background:transparent;}");
    quick_left->resize(35,35);
    quick_left->move(5,7);

    Label* ico_left = new Label(quick_left);
    ico_left->setStyleSheet("background:transparent;");
    QImage Ileft(":/Resources/Navigate/direction-left-0.png");
    ico_left->setPixmap(QPixmap::fromImage(Ileft));
    ico_left->resize(16,16);
    ico_left->setScaledContents(true);
    ico_left->move(10,10);

    quick_left->setEnabled(false);

    //右箭头
    /*quick_right = new Label(navigate);
    quick_right->setStyleSheet("background:transparent;");
    quick_right->resize(35,35);
    quick_right->move(quick_left->x() + quick_left->width() +5,7);

    Label* ico_right = new Label(quick_right);
    ico_right->setStyleSheet("background:transparent;");
    QImage Iright(":/Resources/Navigate/direction-right-0.png");
    ico_right->setPixmap(QPixmap::fromImage(Iright));
    ico_right->resize(16,16);
    ico_right->setScaledContents(true);
    ico_right->move(10,10);

    quick_right->setEnabled(false);*/

    //上箭头
    /*quick_top = new Label(navigate);
    quick_top->setStyleSheet("background:transparent;");
    quick_top->resize(35,35);
    quick_top->move(quick_right->x() + quick_right->width() +5,7);

    Label* ico_up = new Label(quick_top);
    ico_up->setStyleSheet("background:transparent;");
    QImage Iup(":/Resources/Navigate/direction-up-0.png");
    ico_up->setPixmap(QPixmap::fromImage(Iup));
    ico_up->resize(16,16);
    ico_up->setScaledContents(true);
    ico_up->move(10,10);

    quick_top->setEnabled(false);*/

    //刷新
    quick_refresh = new Label(navigate);
    quick_refresh->setStyleSheet("background:transparent;");
    quick_refresh->resize(35,35);
    //quick_refresh->move(quick_top->x() + quick_top->width() +5,7);
    quick_refresh->move(quick_left->x() + quick_left->width() +5,7);

    ico_refresh = new Label(quick_refresh);
    ico_refresh->setStyleSheet("background:transparent;");
    QImage Irefresh(":/Resources/Navigate/refresh-0.png");
    ico_refresh->setPixmap(QPixmap::fromImage(Irefresh));
    ico_refresh->resize(16,16);
    ico_refresh->setScaledContents(true);
    ico_refresh->move(10,10);

    //quick_refresh->setEnabled(false);
    connect(ico_refresh,&Label::clicked,stack_file_explorer,&FileExplorer::Refresh);

    //设置
    quick_setting = new Label(navigate);
    quick_setting->setStyleSheet("background:transparent;");
    quick_setting->resize(35,35);

    Label* ico_setting = new Label(quick_setting);
    ico_setting->setStyleSheet("background:transparent;");
    QImage Isetting(":/Resources/Navigate/setting.png");
    ico_setting->setPixmap(QPixmap::fromImage(Isetting));
    ico_setting->resize(16,16);
    ico_setting->setScaledContents(true);
    ico_setting->move(10,10);

    //搜索
    edit_search = new QLineEdit(navigate);
    edit_search->setObjectName("edit_search");
    edit_search->setPlaceholderText("搜索");
    edit_search->resize(240,32);
    edit_search->setStyleSheet("#edit_search{padding-left:7px;font-size:13px;color:#5E5E5E;border:1px solid #E5E5E5;border-bottom:1px solid #868686;border-radius:5px;background:#FBFBFB;}");

    Label* ico_close = new Label(edit_search);
    ico_close->setStyleSheet("background:transparent;");
    QImage IClose(":/Resources/Common/close.png");
    ico_close->setPixmap(QPixmap::fromImage(IClose));
    ico_close->resize(14,14);
    ico_close->setScaledContents(true);
    ico_close->move(185,9);

    Label* ico_magnifier = new Label(edit_search);
    ico_magnifier->setStyleSheet("background:transparent;");
    QImage Imagnifier(":/Resources/Navigate/search.png");
    ico_magnifier->setPixmap(QPixmap::fromImage(Imagnifier));
    ico_magnifier->resize(16,16);
    ico_magnifier->setScaledContents(true);
    ico_magnifier->move(216,8);

    //地址栏
    urlbar = new QWidget(navigate);
    urlbar->setObjectName("urlbar");
    urlbar->move(quick_refresh->x() + quick_refresh->width() + 5, 9);
    urlbar->setStyleSheet("#urlbar{border:1px solid #E5E5E5;border-bottom:1px solid #868686;border-radius:5px;background:#FBFBFB;}");
}

void StackFileMain::InitAction()
{
    ActionBar = new QWidget(this);
    ActionBar->setObjectName("ActionBar");
    ActionBar->move(navigate->x(), navigate->y()+50);
    ActionBar->setStyleSheet("#ActionBar{background:#F9F9F9;border-bottom:1px solid #EAEAEA;}");

    {
        btn_create = new QPushButton(ActionBar);
        btn_create->setObjectName("create_widget");
        btn_create->move(20,0);
        btn_create->setStyleSheet("#create_widget{border:0px;}#create_widget:hover{border:0px;color: rgb(32, 31, 30);color:#605e5c;background:#E5F3FF;border:1px solid #CCE8FF;}");
        btn_create->resize(88,50);
        btn_create->setEnabled(false);
        btn_create->setCursor(QCursor(Qt::PointingHandCursor));

        //新建文件
        Label* _ico1 = new Label(btn_create);
        _ico1->setStyleSheet("background:transparent;");
        _ico1->setPixmap(QPixmap::fromImage(QImage(":/Resources/Common/add.png")));
        _ico1->move(10,18);

        QLabel* txt1 = new QLabel(btn_create);
        txt1->setText("新建");
        txt1->setStyleSheet("background:transparent;font-family:'微软雅黑';font-size:14px;");
        txt1->move(35,15);

        Label* ico_down = new Label(btn_create);
        ico_down->setStyleSheet("background:transparent;");
        ico_down->setPixmap(QPixmap::fromImage(QImage(":/Resources/Common/down.png")));
        ico_down->move(btn_create->width()-20,18);

        connect(btn_create,&QPushButton::clicked,stack_file_explorer,&FileExplorer::OpenCreateDropDown);
        connect(_ico1,&Label::clicked,stack_file_explorer,&FileExplorer::OpenCreateDropDown);
        connect(ico_down,&Label::clicked,stack_file_explorer,&FileExplorer::OpenCreateDropDown);
    }

    {
        btn_upload = new QPushButton(ActionBar);
        btn_upload->setObjectName("upload_widget");
        btn_upload->move(20 + btn_create->width() +5,0);
        btn_upload->setStyleSheet("#upload_widget{border:0px;}#upload_widget:hover{border:0px;color: rgb(32, 31, 30);color:#605e5c;background:#E5F3FF;border:1px solid #CCE8FF;}");
        btn_upload->resize(88,50);
        btn_upload->setEnabled(false);
        btn_upload->setCursor(QCursor(Qt::PointingHandCursor));

        Label* ico_up = new Label(btn_upload);
        ico_up->setStyleSheet("background:transparent;");
        ico_up->setPixmap(QPixmap::fromImage(QImage(":/Resources/Common/upload.png")));
        ico_up->move(10,20);

        QLabel* txt_up = new QLabel(btn_upload);
        txt_up->setText("上传");
        txt_up->setStyleSheet("background:transparent;font-family:'微软雅黑';font-size:14px;");
        txt_up->move(35,15);

        Label* ico_down_up = new Label(btn_upload);
        ico_down_up->setStyleSheet("background:transparent;");
        ico_down_up->setPixmap(QPixmap::fromImage(QImage(":/Resources/Common/down.png")));
        ico_down_up->move(btn_upload->width()-20,18);

        connect(btn_upload,&QPushButton::clicked,stack_file_explorer,&FileExplorer::OpenUploadDropDown);
        connect(ico_up,&Label::clicked,stack_file_explorer,&FileExplorer::OpenUploadDropDown);
        connect(ico_down_up,&Label::clicked,stack_file_explorer,&FileExplorer::OpenUploadDropDown);
    }


    //sort 36*36
    ActSore = new Label(ActionBar);
    ActSore->setStyleSheet("background:transparent;");
    ActSore->resize(36,36);

    Label* ico_sort = new Label(ActSore);
    ico_sort->setStyleSheet("background:transparent;");
    QImage Isort(":/Resources/Common/sort.png");
    ico_sort->setPixmap(QPixmap::fromImage(Isort));
    ico_sort->resize(18,18);
    ico_sort->setScaledContents(true);
    ico_sort->move(9,9);

    //layout 36*36
    ActLayout = new Label(ActionBar);
    ActLayout->setStyleSheet("background:transparent;");
    ActLayout->resize(36,36);

    Label* ico_layout = new Label(ActLayout);
    ico_layout->setStyleSheet("background:transparent;");
    QImage Ilayout(":/Resources/Common/layout.png");
    ico_layout->setPixmap(QPixmap::fromImage(Ilayout));
    ico_layout->resize(18,18);
    ico_layout->setScaledContents(true);
    ico_layout->move(9,9);

    //more
    ActMore = new Label(ActionBar);
    ActMore->setStyleSheet("background:transparent;");
    ActMore->resize(36,36);

    Label* ico_more = new Label(ActMore);
    ico_more->setStyleSheet("background:transparent;");
    QImage Imore(":/Resources/Common/more.png");
    ico_more->setPixmap(QPixmap::fromImage(Imore));
    ico_more->resize(22,22);
    ico_more->setScaledContents(true);
    ico_more->move(7,7);
}

void StackFileMain::InitContent()
{
    content = new QStackedWidget(this);
    content->setObjectName("content");
    content->move(0,100);

    content->addWidget(stack_file_welcome);
    content->addWidget(stack_file_explorer);
    content->setCurrentWidget(stack_file_welcome);

    /**
     * 点击了部门和群组文件后的逻辑
     * 更新Stack
     * 更新url地址栏
     */
    connect(stack_file_welcome,&FileWelcome::explorer,this,[=](QString category,QString key,QString text,int id){
        fd->id = 0;

        UrlMeta* meta = new UrlMeta();
        meta->key = key;
        meta->text = text;
        meta->category = category;
        meta->id = id;
        UrlMetas.append(meta);

        content->setCurrentWidget(stack_file_explorer);
        stack_file_explorer->set_meta(meta);
        stack_file_explorer->flush(content->width(),content->height());
        wait(50);
        stack_file_explorer->OnStart();

        render_urlbar();

        btn_create->setEnabled(true);
        btn_upload->setEnabled(true);

        quick_refresh->setEnabled(true);
        ico_refresh->setPixmap(QPixmap::fromImage(QImage(":/Resources/Navigate/refresh-1.png")));
        ico_refresh->setCursor(Qt::PointingHandCursor);
    });
}

void StackFileMain::resizeEvent(QResizeEvent *)
{
    navigate->resize(this->width(),50);
    quick_setting->move(navigate->width() - 40,7);
    edit_search->move(quick_setting->x() - 245,9);
    urlbar->resize(this->width()-385,32);

    //动作栏
    ActionBar->resize(navigate->width(),50);

    ActSore->move(ActionBar->width() - 123,7);
    ActLayout->move(ActionBar->width() - 82,7);
    ActMore->move(ActionBar->width() - 41,7);

    //内容区域
    content->resize(this->width(),this->height() - 100);
    content->currentWidget()->resize(content->width(),content->height());

    if(stack_file_explorer == content->currentWidget())
    {
        stack_file_explorer->flush(content->width(),content->height());
    }
    if(stack_file_welcome == content->currentWidget())
    {
        stack_file_welcome->flush(content->width(),content->height());
    }
}

void StackFileMain::InitUrlBar()
{
    url_layout = new QHBoxLayout(urlbar);
    url_layout->setMargin(0);
    url_layout->setSpacing(4);
    urlbar->setLayout(url_layout);

    UrlMeta* home = new UrlMeta();
    home->key = "HOME";
    home->text = "首页";
    home->category = "SYSTEM";
    home->id = 0;
    UrlMetas.append(home);

    //渲染地址栏
    render_urlbar();
}

void StackFileMain::render_urlbar()
{
    QLayoutItem *child;
    while ((child = url_layout->takeAt(0)) != 0)
    {
        if(child->widget())
        {
            child->widget()->setParent(NULL);
        }
        delete child;
    }
    QLabel* startSpace = new QLabel(urlbar);
    startSpace->setFixedWidth(5);
    url_layout->addWidget(startSpace);

    for (int i=0; i<UrlMetas.size(); i++) {
        Label* meta = new Label(urlbar);
        meta->setObjectName("url"+UrlMetas[i]->key);
        meta->setFixedHeight(30);
        meta->setStyleSheet("#url"+UrlMetas[i]->key+"{padding-left:1px;color:#605e5c;background:transparent;font-size: 14px;font-family: \"Microsoft Yahei UI\";}#url"+UrlMetas[i]->key+":hover{color:#605e5c;background:#E5F3FF;border:1px solid #CCE8FF;font-size: 14px;font-family: \"Microsoft Yahei UI\";}");
        meta->setCursor(QCursor(Qt::PointingHandCursor));

        //meta->setFixedWidth(strlen(UrlMetas[i]->text)+20);

        Label* txt = new Label(meta);
        txt->setText(UrlMetas[i]->text);
        txt->setFixedHeight(30);
        txt->adjustSize();
        txt->move(5,0);

        Label* sp = new Label(meta);
        sp->resize(10,30);
        sp->setPixmap(QPixmap::fromImage(QImage(":/Resources/Navigate/r.png")));
        sp->move(txt->width()+5,0);

        meta->setFixedWidth(txt->width() + sp->width()+10);

        url_layout->addWidget(meta);

        connect(txt,&Label::clicked,this,[this,i](){
            this->url_meta_clicked(UrlMetas[i]);
        });
    }
    QSpacerItem *si = new QSpacerItem(0,0,QSizePolicy::Expanding);
    url_layout->addSpacerItem(si);
}

//点击了地址栏
void StackFileMain::url_meta_clicked(UrlMeta* meta)
{
    //当点击了首页
    if("HOME" == meta->key)
    {
        UrlMetas.clear();
        fd->id = 0;

        UrlMeta* home = new UrlMeta();
        home->key = "HOME";
        home->text = "首页";
        home->category = "SYSTEM";
        home->id = 0;
        UrlMetas.append(home);
        content->setCurrentWidget(stack_file_welcome);

        btn_create->setEnabled(false);
        btn_upload->setEnabled(false);
    }
    else
    {
        if("SYS" == meta->category)
        {
            UrlMetas.clear();
            fd->id = 0;

            UrlMeta* home = new UrlMeta();
            home->key = "HOME";
            home->text = "首页";
            home->category = "SYSTEM";
            home->id = 0;
            UrlMetas.append(home);

            UrlMetas.append(meta);
            stack_file_explorer->set_meta(meta);
        }
        else
        {
            QVector<UrlMeta*> ums = UrlMetas;
            UrlMetas.clear();
            for (int i=0; i<ums.count(); i++) {
                UrlMetas.append(ums[i]);
                if(meta->key == ums[i]->key){
                    break;
                }
            }
            stack_file_explorer->change_folder(meta);
        }

        btn_create->setEnabled(true);
        btn_upload->setEnabled(true);
    }
    render_urlbar();
}

//追加地址栏
void StackFileMain::append_urlbar(FD* selected_fd)
{
    UrlMeta* meta = new UrlMeta();
    meta->key = "folder"+QString::number(selected_fd->id);
    meta->text = selected_fd->name;
    meta->category = "folder";
    meta->id = selected_fd->id;
    UrlMetas.append(meta);

    render_urlbar();
}

