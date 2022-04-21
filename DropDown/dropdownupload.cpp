#pragma execution_character_set("utf-8")
#include "dropdownupload.h"
#include "ui_dropdownupload.h"

#include <QLabel>
#include <QPushButton>

DropDownUpload::DropDownUpload(QWidget *parent) : QDialog(parent),ui(new Ui::DropDownUpload)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    init();
}

DropDownUpload::~DropDownUpload()
{
    delete ui;
}

void DropDownUpload::init()
{
    FILE_FILTER* file = new FILE_FILTER();
    file->objname = "file";
    file->txt = "文件";
    file->ico = "Action_New_32x32";
    file->css = "border-bottom:1px solid #EFF0F1;";
    filters.append(file);

    FILE_FILTER* folder = new FILE_FILTER();
    folder->objname = "folder";
    folder->txt = "文件夹";
    folder->ico = "PackageProduct_32x32";
    filters.append(folder);

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
        QImage img(":/Resources/icon/"+filters[i]->ico+".png");
        _ico->setPixmap(QPixmap::fromImage(img));
        _ico->resize(16,16);
        _ico->setScaledContents(true);
        _ico->move(8,10);

        QLabel* txt = new QLabel(btn);
        txt->setText(filters[i]->txt);
        txt->setStyleSheet("background:transparent;font-family:'微软雅黑';font-size:14px;");
        txt->move(30,8);


        QLabel* line = new QLabel(this);
        line->resize(this->width()-10,1);
        line->setStyleSheet("background:#edebe9;");
        line->move(5,btn->y()+btn->height());

        connect(btn,&QPushButton::clicked,this,[=]() {emit IntentUpload(filters[i]->objname);});
    }
}
