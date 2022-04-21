#pragma execution_character_set("utf-8")
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include <QStyleOption>
#include <QPainterPath>
#include <QDebug>
#include <QScreen>
#include <QScreen>
#include "BaseWindow.h"
#include "TitleBar.h"

BaseWindow::BaseWindow(QWidget *parent) : BaseController(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    setAttribute(Qt::WA_TranslucentBackground);
    initTitleBar();

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    H = screenGeometry.height();
    W = screenGeometry.width();
}


BaseWindow::~BaseWindow()
{

}

void BaseWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
    return QWidget::paintEvent(event);
}

void BaseWindow::loadStyleSheet(const QString &sheetName)
{
    QFile file(":/Resources/" + sheetName + ".css");
    file.open(QFile::ReadOnly);
    if (file.isOpen())
    {
        QString styleSheet = this->styleSheet();
        styleSheet += file.readAll();
        this->setStyleSheet(styleSheet);
    }
}

void BaseWindow::initTitleBar()
{
    m_TitleBar = new TitleBar(this);
    m_TitleBar->move(this->border_width,this->border_width);

    connect(m_TitleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));     // 最小化
    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked())); // 关闭
    connect(m_TitleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));     // 最大化
    connect(m_TitleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked())); //还原
}

void BaseWindow::onButtonMaxClicked()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->availableGeometry();
    int width = screenGeometry.width() + this->border_width*2;
    int height = screenGeometry.height() -  - this->border_width*2;

    m_TitleBar->saveRestoreInfo(this->pos(), QSize(this->width(), this->height()));
    m_TitleBar->set_width(width);
    this->move(0-this->border_width,0-this->border_width);
    this->setFixedSize(QSize(width,height));
}

void BaseWindow::onButtonRestoreClicked()
{
    QPoint windowPos;
    QSize windowSize;
    m_TitleBar->getRestoreInfo(windowPos, windowSize);
    m_TitleBar->set_width(windowSize.width());
    this->move(windowPos);
    this->setFixedSize(windowSize);
}

void BaseWindow::onButtonMinClicked()
{
    if (Qt::Tool == (windowFlags() & Qt::Tool))
    {
        hide();    //设置了Qt::Tool 如果调用showMinimized()则窗口就销毁了？？？
    }
    else
    {
        showMinimized();
    }
}

void BaseWindow::onButtonCloseClicked()
{
    close();
}
