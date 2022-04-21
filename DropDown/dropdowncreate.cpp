#pragma execution_character_set("utf-8")
#include "dropdowncreate.h"
#include "ui_dropdowncreate.h"

#include <QLabel>
#include <QPushButton>

DropDownCreate::DropDownCreate(QWidget *parent) :QDialog(parent),ui(new Ui::DropDownCreate)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    init();
}

DropDownCreate::~DropDownCreate()
{
    delete ui;
}

void DropDownCreate::init()
{
    FILE_FILTER* folder = new FILE_FILTER();
    folder->objname = "folder";
    folder->txt = "文件夹";
    folder->ico = "cfolder";
    folder->css = "border-bottom:1px solid #EFF0F1;";
    filters.append(folder);

    FILE_FILTER* txt = new FILE_FILTER();
    txt->objname = "txt";
    txt->txt = "纯文本文档";
    txt->ico = "txt";
    filters.append(txt);

    this->resize(this->size().width(),36*filters.length()+10);

    for (int i=0; i<filters.length(); i++)
    {
        QPushButton* btn = new QPushButton(this);
        btn->setObjectName(filters[i]->objname);
        btn->move(5,36 * i + 5);
        btn->setStyleSheet("#"+filters[i]->objname+"{background:#fff;border:none;"+filters[i]->css+"}#"+filters[i]->objname+":hover{border:0px;"+filters[i]->css+"background-color:#f3f2f1;color: rgb(32, 31, 30);}");
        btn->resize(this->width()-10,36);
        btn->setCursor(QCursor(Qt::PointingHandCursor));

        QLabel* _ico = new QLabel(btn);
        _ico->setStyleSheet("background:transparent;");
        QImage img(":/Resources/16x16/"+filters[i]->ico+".png");
        _ico->setPixmap(QPixmap::fromImage(img));
        _ico->resize(16,16);
        _ico->move(8,10);

        QLabel* txt = new QLabel(btn);
        txt->setText(filters[i]->txt);
        txt->setStyleSheet("background:transparent;font-family:'微软雅黑';font-size:14px;");
        txt->move(30,8);


        QLabel* line = new QLabel(this);
        line->resize(this->width()-10,1);
        line->setStyleSheet("background:#edebe9;");
        line->move(5,btn->y()+btn->height());

        connect(btn,&QPushButton::clicked,this,[=]() {emit IntentOpenCreateDialog(filters[i]->objname);});
    }
}
