#pragma execution_character_set("utf-8")
#include "menufileexplorercanvas.h"
#include "ui_menufileexplorercanvas.h"

#include <QLabel>
#include <QPushButton>

MenuFileExplorerCanvas::MenuFileExplorerCanvas(QWidget *parent) : QDialog(parent), ui(new Ui::MenuFileExplorerCanvas)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    render_data();
    render_ui();
}

MenuFileExplorerCanvas::~MenuFileExplorerCanvas()
{
    delete ui;
}

void MenuFileExplorerCanvas::render_data()
{
    MenuItem* cfolder = new MenuItem();
    cfolder->text = "新建目录";
    cfolder->key = "cfolder";
    cfolder->ico = "16x16/cfolder.png";
    menus.append(cfolder);

    /*MenuItem* ctxt = new MenuItem();
    ctxt->text = "新建文本文件";
    ctxt->key = "ctxt";
    ctxt->ico = "16x16/txt.png";
    ctxt->border = "border-bottom:1px solid #EFF0F1;";
    menus.append(ctxt);*/

    MenuItem* upload_file = new MenuItem();
    upload_file->text = "上传文件";
    upload_file->key = "upload_file";
    upload_file->ico = "icon/Action_New_32x32";
    menus.append(upload_file);

    MenuItem* upload_folder = new MenuItem();
    upload_folder->text = "上传文件夹";
    upload_folder->key = "upload_folder";
    upload_folder->ico = "icon/PackageProduct_32x32";
    upload_folder->border = "border-bottom:1px solid #EFF0F1;";
    menus.append(upload_folder);

    MenuItem* refresh = new MenuItem();
    refresh->text = "刷新";
    refresh->key = "refresh";
    refresh->ico = "16x16/refresh.png";
    menus.append(refresh);
}

void MenuFileExplorerCanvas::render_ui()
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
        _ico->setScaledContents(true);
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
