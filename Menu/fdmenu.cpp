#pragma execution_character_set("utf-8")

#include "fdmenu.h"
#include "ui_fdmenu.h"

#include <QLabel>
#include <QPushButton>

FdMenu::FdMenu(QWidget *parent) : QDialog(parent),ui(new Ui::FdMenu)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    render_data();
    render_ui();
}

FdMenu::~FdMenu()
{
    delete ui;
}


void FdMenu::render_data()
{
    MenuItem* open = new MenuItem();
    open->text = "打开";
    open->key = "open";
    open->ico = "16x16/open.png";

    MenuItem* location = new MenuItem();
    location->text = "定位文件";
    location->key = "location";
    location->ico = "16x16/location.png";
    location->border = "border-bottom:1px solid #EFF0F1;";

    MenuItem* ding = new MenuItem();
    ding->text = "发送给钉钉同事";
    ding->key = "ding";
    ding->ico = "16x16/ding.png";

    MenuItem* share = new MenuItem();
    share->text = "共享";
    share->key = "share";
    share->ico = "16x16/share.png";
    share->border = "border-bottom:1px solid #EFF0F1;";

    MenuItem* move = new MenuItem();
    move->text = "移动";
    move->key = "move";
    move->ico = "16x16/move.png";

    MenuItem* rename = new MenuItem();
    rename->text = "重命名";
    rename->key = "rename";
    rename->ico = "16x16/rename.png";

    MenuItem* del = new MenuItem();
    del->text = "删除";
    del->key = "del";
    del->ico = "16x16/del.png";
    del->color = "#ff0000";
    del->border = "border-bottom:1px solid #EFF0F1;";

    MenuItem* attr = new MenuItem();
    attr->text = "属性";
    attr->key = "attr";
    attr->ico = "16x16/attr.png";

    menus.append(open);
    menus.append(location);
    menus.append(ding);
    menus.append(share);
    menus.append(move);
    menus.append(rename);
    menus.append(del);
    menus.append(attr);
}

void FdMenu::render_ui()
{
    this->resize(this->size().width(),36*menus.length()+10);

    for (int i=0; i<menus.length(); i++)
    {
        QPushButton* btn = new QPushButton(this);
        btn->setObjectName(menus[i]->key);
        btn->move(5,36 * i + 5);
        btn->resize(this->width()-10,36);
        btn->setCursor(QCursor(Qt::PointingHandCursor));
        btn->setStyleSheet("#"+menus[i]->key+"{background:#fff;border:0px;"+menus[i]->border+"}#"+menus[i]->key+":hover{border:0px;background-color:#f3f2f1;color: rgb(32, 31, 30);border:0px;"+menus[i]->border+"}");
        btn->show();

        QLabel* _ico = new QLabel(btn);
        _ico->setStyleSheet("background:transparent;");
        QImage img(":/Resources/"+menus[i]->ico);
        _ico->setPixmap(QPixmap::fromImage(img));
        _ico->resize(16,16);
        _ico->move(8,10);
        _ico->show();

        QLabel* txt = new QLabel(btn);
        txt->setText(menus[i]->text);
        txt->setStyleSheet("background:transparent;font-family:'微软雅黑';font-size:14px;color:"+menus[i]->color+";");
        txt->move(30,8);
        txt->show();


        connect(btn,&QPushButton::clicked,this,[=]() {
            emit menu_clicked(menus[i]->key);
        });
    }
}
