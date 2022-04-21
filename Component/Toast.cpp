#include "Toast.h"
#include "ui_Toast.h"
#include <QPropertyAnimation>
#include <QScreen>
#include <QGuiApplication>
#include <QPainter>
#include <QTimer>


Toast::Toast(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Toast)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, true);
}

Toast::~Toast()
{
    delete ui;
}


void Toast::setText(const QString& text)
{
    ui->label->setText(text);
}

void Toast::setBgColor(const QString &color)
{
    QString css = "border-radius: 26px;color: #FFFFFF;font-family: microsoft yahei;font-size: 16px;padding-left:25px;padding-right:25px;";
    if("ERROR" == color)
    {
        ui->label->setStyleSheet("background-color: #dc3545;"+css);
    }else if("SUCC" == color)
    {
        ui->label->setStyleSheet("background-color: #198754;"+css);
    }
    else if("WARNING" == color)
    {
        ui->label->setStyleSheet("background-color: #fd7e14;"+css);
    }
    else if("INFOMATION" == color)
    {
        ui->label->setStyleSheet("background-color: #009ef6;"+css);
    }
    else{
        ui->label->setStyleSheet("background-color: rgba(0,0,0,0.80);"+css);
    }
}

void Toast::showAnimation(int timeout)
{
    // 开始动画
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
    show();

    QTimer::singleShot(timeout, [&]
    {
        // 结束动画
        QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
        animation->setDuration(1000);
        animation->setStartValue(1);
        animation->setEndValue(0);
        animation->start();
        connect(animation, &QPropertyAnimation::finished, [&]
        {
            close();
            deleteLater();// 关闭后析构
        });
    });
}

void Toast::tip(const QString &text)
{
    display(text,"TIP");
}

void Toast::info(const QString &text)
{
    display(text,"INFOMATION");
}

void Toast::warn(const QString &text)
{
    display(text,"WARNING");
}

void Toast::err(const QString &text)
{
    display(text,"ERROR");
}

void Toast::succ(const QString &text)
{
    display(text,"SUCC");
}


void Toast::display(const QString& text,QString color)
{
    Toast* toast = new Toast(nullptr);
    toast->setWindowFlags(toast->windowFlags() | Qt::WindowStaysOnTopHint); // 置顶
    toast->setText(text);
    toast->setBgColor(color);
    toast->adjustSize();


    // 测试显示位于主屏的70%高度位置
    QScreen* pScreen = QGuiApplication::primaryScreen();
    toast->move((pScreen->size().width() - toast->width()) / 2, pScreen->size().height() * 7 / 10);
    toast->showAnimation();
}

void Toast::paintEvent(QPaintEvent *event)
{

}
