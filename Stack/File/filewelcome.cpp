#pragma execution_character_set("utf-8")
#include "filewelcome.h"
#include <QDebug>
#include <math.h>
#include <Component/Label.h>

FileWelcome::FileWelcome(QWidget *parent) : QWidget(parent)
{
    //我的文件
    draw_myfile();

    //部门文件
    draw_depfile();

    //群组文件
    draw_groupfile();

    //右键
    this->setContextMenuPolicy(Qt::CustomContextMenu); //给内容区域增加右键菜单
    this->setObjectName("FileWelcome");
    this->installEventFilter(this);
    menu = new FileWelcomeMenu(this);
    menu->hide();
    connect(this,&QWidget::customContextMenuRequested,this,[=](const QPoint &pos)
    {
        //menu->move(pos.x(),pos.y());
        //menu->show();
    });
    //connect(menu,SIGNAL(select_menu(QString)),this,SLOT(select_menu(QString)));
}


//我的文件
void FileWelcome::draw_myfile()
{
    myfile_wapper = new QWidget(this);
    myfile_wapper->setObjectName("myfile_wapper");
    myfile_wapper->move(20,20);
    myfile_wapper->resize(width,100);
    //myfile_wapper->setStyleSheet("#myfile_wapper{background:red;}");

    QLabel* label_me = new QLabel(myfile_wapper);
    label_me->setObjectName("label_me");
    label_me->move(0,0);
    label_me->setText("我的文件");
    label_me->setStyleSheet("#label_me{color:#1A1A1A;font-size:14px;font-weight:bold;}");

    myfile = new Label(myfile_wapper);
    myfile->setObjectName("myfile");
    myfile->move(0,30);
    myfile->resize(195,64);
    myfile->setCursor(QCursor(Qt::PointingHandCursor));
    myfile->setStyleSheet("#myfile{width:100px;height:88px;background:#FDFDFD;border:1px solid #EAEAEA;border-radius: 5px;border-bottom:1px solid #D1D1D1;}#myfile:hover{background:#E5F3FF}");
    myfile->installEventFilter(this);
    connect(myfile,&Label::dbclicked,this,[=](){
        emit explorer("SYS","MY_FILE","我的文件",0);
    });

    Label* ico_myfile = new Label(myfile);
    ico_myfile->setStyleSheet("background:transparent;");
    QImage Imyfile(":/Resources/fd/myfile.ico");
    ico_myfile->setPixmap(QPixmap::fromImage(Imyfile));
    ico_myfile->resize(40,40);
    ico_myfile->setScaledContents(true);
    ico_myfile->move(10,12);

    QLabel* tip_myfile = new QLabel(myfile);
    tip_myfile->setObjectName("tip_myfile");
    tip_myfile->setText("个人文件");
    tip_myfile->move(60,10);
    tip_myfile->setStyleSheet("#tip_myfile{font-size:14px;color:#000;}");

    QLabel* tip_myfile_size = new QLabel(myfile);
    tip_myfile_size->setObjectName("tip_myfile_size");
    tip_myfile_size->setText("共931.51GB");
    tip_myfile_size->move(60,35);
    tip_myfile_size->setStyleSheet("#tip_myfile_size{font-size:12px;color:#999;}");

    //分享出去的
    share_out = new Label(this);
    share_out->setObjectName("share_out");
    share_out->move(240,50);
    share_out->resize(195,64);
    share_out->setCursor(QCursor(Qt::PointingHandCursor));
    share_out->setStyleSheet("#share_out{width:100px;height:88px;background:#FDFDFD;border:1px solid #EAEAEA;border-radius: 5px;border-bottom:1px solid #D1D1D1;}#share_out:hover{background:#E5F3FF}");
    share_out->installEventFilter(this);
    connect(share_out,&Label::dbclicked,this,[=](){
        emit explorer("SYS","SHARE_OUT","共享出的文件",0);
    });

    Label* ico_share_out = new Label(share_out);
    ico_share_out->setStyleSheet("background:transparent;");
    QImage src_share_out(":/Resources/fd/share_out.ico");
    ico_share_out->setPixmap(QPixmap::fromImage(src_share_out));
    ico_share_out->resize(40,40);
    ico_share_out->setScaledContents(true);
    ico_share_out->move(10,12);

    QLabel* tip_share_out = new QLabel(share_out);
    tip_share_out->setObjectName("tip_share_out");
    tip_share_out->setText("我分享的文件");
    tip_share_out->move(60,10);
    tip_share_out->setStyleSheet("#tip_share_out{font-size:14px;color:#000;}");

    QLabel* tip_number_share_out = new QLabel(share_out);
    tip_number_share_out->setObjectName("tip_number_out");
    tip_number_share_out->setText("共931.51GB");
    tip_number_share_out->move(60,35);
    tip_number_share_out->setStyleSheet("#tip_number_out{font-size:12px;color:#999;}");

    //分享给我的
    share_in = new Label(this);
    share_in->setObjectName("share_in");
    share_in->move(460,50);
    share_in->resize(195,64);
    share_in->setCursor(QCursor(Qt::PointingHandCursor));
    share_in->setStyleSheet("#share_in{width:100px;height:88px;background:#FDFDFD;border:1px solid #EAEAEA;border-radius: 5px;border-bottom:1px solid #D1D1D1;}#share_in:hover{background:#E5F3FF}");
    share_in->installEventFilter(this);
    connect(share_in,&Label::dbclicked,this,[=](){
        emit explorer("SYS","SHARE_IN","共享给我的文件",0);
    });

    Label* ico_share_in = new Label(share_in);
    ico_share_in->setStyleSheet("background:transparent;");
    QImage src_share_in(":/Resources/fd/share_in.ico");
    ico_share_in->setPixmap(QPixmap::fromImage(src_share_in));
    ico_share_in->resize(40,40);
    ico_share_in->setScaledContents(true);
    ico_share_in->move(10,12);

    QLabel* tip_share_in = new QLabel(share_in);
    tip_share_in->setObjectName("tip_share_in");
    tip_share_in->setText("分享给我的文件");
    tip_share_in->move(60,10);
    tip_share_in->setStyleSheet("#tip_share_in{font-size:14px;color:#000;}");

    QLabel* tip_number_share_in = new QLabel(share_in);
    tip_number_share_in->setObjectName("tip_number_in");
    tip_number_share_in->setText("共931.51GB");
    tip_number_share_in->move(60,35);
    tip_number_share_in->setStyleSheet("#tip_number_in{font-size:12px;color:#999;}");
}

//部门文件
void FileWelcome::draw_depfile()
{
    QLabel* label_dep = new QLabel(this);
    label_dep->setObjectName("label_dep");
    label_dep->move(myfile_wapper->x(),myfile_wapper->y() + myfile_wapper->height() + 40);
    label_dep->setText("部门文件");
    label_dep->setStyleSheet("#label_dep{color:#1A1A1A;font-size:14px;font-weight:bold;}");

    depfile_wapper = new QWidget(this);
    depfile_wapper->setObjectName("depfile_wapper");
    depfile_wapper->move(label_dep->x(),label_dep->y() + 30);
    //depfile_wapper->setStyleSheet("#depfile_wapper{background:green;}");

    dep_layout = new FlowLayout(this);
    dep_layout->setMargin(7);
    depfile_wapper->setLayout(dep_layout);

    for (int i=0; i<10; i++) {
        SIMPLE* _dep = new SIMPLE();
        _dep->title = "部门"+QString::number(i);
        _dep->id = QString::number(i);
        deps.append(_dep);
    }
}


void FileWelcome::render_deps()
{
    int total = deps.count();
    int row_count = floor((width - 20) / 200);
    int row = ceil(total*1.0 / row_count*1.0);
    depfile_wapper->resize(depfile_wapper->width(),70*row+10);

    for (int i=0; i<total; i++)
    {

        Label *dep = new Label(depfile_wapper);
        dep->setObjectName("dep_"+QString::number(i));
        dep->setMinimumSize(195,64);
        dep->setCursor(QCursor(Qt::PointingHandCursor));
        dep->setStyleSheet("#dep_"+QString::number(i)+"{width:100px;height:88px;background:#FDFDFD;border:1px solid #EAEAEA;border-radius: 5px;border-bottom:1px solid #D1D1D1;}#dep_"+QString::number(i)+":hover{background:#E5F3FF}");
        dep->installEventFilter(this);
        connect(dep,&Label::dbclicked,this,[=](){
            emit explorer("SYS","DEP_"+QString::number(i),"部门文件"+QString::number(i),i);
        });

        Label* ico = new Label(dep);
        ico->setStyleSheet("background:transparent;");
        QImage src(":/Resources/fd/dep.ico");
        ico->setPixmap(QPixmap::fromImage(src));
        ico->resize(40,40);
        ico->setScaledContents(true);
        ico->move(10,12);

        QLabel* title = new QLabel(dep);
        title->setObjectName("title"+QString::number(i));
        title->setText("部门文件"+QString::number(i));
        title->move(60,10);
        title->setStyleSheet("#title"+QString::number(i)+"{font-size:14px;color:#000;}");

        QLabel* dep_meta = new QLabel(dep);
        dep_meta->setObjectName("tip_number_out");
        dep_meta->setText("共931.51GB");
        dep_meta->move(60,35);
        dep_meta->setStyleSheet("#tip_number_out{font-size:12px;color:#999;}");

        dep_layout->addWidget(dep);
    }
}

//群组文件
void FileWelcome::draw_groupfile()
{
    label_group = new QLabel(this);
    label_group->setObjectName("label_dep");
    label_group->setText("群组文件");
    label_group->setStyleSheet("#label_dep{color:#1A1A1A;font-size:14px;font-weight:bold;}");

    groupfile_wapper = new QWidget(this);
    groupfile_wapper->setObjectName("groupfile_wapper");
    groupfile_wapper->resize(width,100);
    //groupfile_wapper->setStyleSheet("#groupfile_wapper{background:yellow;}");

    group_layout = new FlowLayout(this);
    group_layout->setMargin(7);
    groupfile_wapper->setLayout(group_layout);

    for (int i=0; i<10; i++) {
        SIMPLE* _group = new SIMPLE();
        _group->title = "群组"+QString::number(i);
        _group->id = QString::number(i);
        groups.append(_group);
    }
}

void FileWelcome::clear_fd(FlowLayout* layout)
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != 0)
    {
        if(child->widget())
        {
            child->widget()->setParent(NULL);
        }

        delete child;
    }
}

bool FileWelcome::eventFilter(QObject *target, QEvent *event)
{
    if(QEvent::ContextMenu == event->type())
    {

        //qDebug() << "target->objectName()=" <<target->objectName();

    }
    return QWidget::eventFilter(target,event);
}


void FileWelcome::render_groups()
{
    int total = groups.count();
    int row_count = floor((width - 20) / 200);
    int row = ceil(total*1.0 / row_count*1.0);
    groupfile_wapper->resize(groupfile_wapper->width(),70*row+10);

    label_group->move(depfile_wapper->x(),depfile_wapper->y() + depfile_wapper->height() + 40);
    groupfile_wapper->move(label_group->x(),label_group->y() + 30);

    for (int i=0; i<total; i++)
    {

        Label *_group = new Label(groupfile_wapper);
        _group->setObjectName("group_"+QString::number(i));
        _group->setMinimumSize(195,64);
        _group->setCursor(QCursor(Qt::PointingHandCursor));
        _group->setStyleSheet("#group_"+QString::number(i)+"{width:100px;height:88px;background:#FDFDFD;border:1px solid #EAEAEA;border-radius: 5px;border-bottom:1px solid #D1D1D1;}#group_"+QString::number(i)+":hover{background:#E5F3FF}");
        _group->installEventFilter(this);
        connect(_group,&Label::dbclicked,this,[=](){
            emit explorer("SYS","GROUP_"+QString::number(i),"群组文件"+QString::number(i),i);
        });

        Label* ico = new Label(_group);
        ico->setStyleSheet("background:transparent;");
        QImage src(":/Resources/fd/group.ico");
        ico->setPixmap(QPixmap::fromImage(src));
        ico->resize(40,40);
        ico->setScaledContents(true);
        ico->move(10,12);

        QLabel* title = new QLabel(_group);
        title->setObjectName("title"+QString::number(i));
        title->setText("群组文件"+QString::number(i));
        title->move(60,10);
        title->setStyleSheet("#title"+QString::number(i)+"{font-size:14px;color:#000;}");

        QLabel* dep_meta = new QLabel(_group);
        dep_meta->setObjectName("tip_number_out");
        dep_meta->setText("共931.51GB");
        dep_meta->move(60,35);
        dep_meta->setStyleSheet("#tip_number_out{font-size:12px;color:#999;}");

        group_layout->addWidget(_group);
    }
}


void FileWelcome::flush(int w,int h)
{
    this->width = w;

    myfile_wapper->resize(w,100);

    depfile_wapper->resize(w-40,100);
    groupfile_wapper->resize(w-40,100);

    clear_fd(dep_layout);
    render_deps();

    clear_fd(group_layout);
    render_groups();
}
