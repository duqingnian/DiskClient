#pragma execution_character_set("utf-8")
#include <QHBoxLayout>
#include <QPixmap>
#include <QPaintEvent>
#include <QPainter>
#include <QFont>
#include <QFontDatabase>
#include <QFile>
#include <QPainterPath>
#include <QDebug>
#include "TitleBar.h"

#define BUTTON_HEIGHT 27		// 按钮高度;
#define BUTTON_WIDTH 27			// 按钮宽度;
#define TITLE_HEIGHT 35			// 标题栏高度;

TitleBar::TitleBar(QWidget *parent) : QWidget(parent)
{
    m_colorR = 255;
    m_colorG = 255;
    m_colorB = 255;

    m_windowBorderWidth = 0;

    initControl();
    initConnections();

    m_isPressed = false;

    title_width = 300;
}

void TitleBar::initControl()
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    //标题
    m_pTitle = new QLabel;
    m_pTitle->setText("");
    m_pTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pTitle->setStyleSheet("font-size:14px;font-weight:bold;margin-left:10px;font-family:Microsoft YaHei;");
    layout->addWidget(m_pTitle);

    m_pButtonSetting = new QPushButton;
    m_pButtonMin = new QPushButton;
    m_pButtonMax = new QPushButton;
    m_pButtonClose = new QPushButton;
    m_pButtonRestore = new QPushButton;

    m_pButtonSetting->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

    m_pButtonSetting->setToolTip("设置");
    m_pButtonMin->setToolTip("最小化");
    m_pButtonMax->setToolTip("最大化");
    m_pButtonClose->setToolTip("关闭");
    m_pButtonRestore->setToolTip("还原");

    m_pButtonSetting->setObjectName("ButtonSetting");
    m_pTitle->setObjectName("TitleContent");
    m_pButtonMin->setObjectName("ButtonMin");
    m_pButtonMax->setObjectName("ButtonMax");
    m_pButtonClose->setObjectName("ButtonClose");
    m_pButtonRestore->setObjectName("ButtonRestore");

    m_pButtonSetting->setVisible(false);
    m_pButtonMax->setVisible(false);
    m_pButtonRestore->setVisible(false);

    label_version = new Label(this);
    label_version->setVisible(false);

    layout->addWidget(m_pButtonSetting);
    layout->addWidget(m_pButtonMin);
    layout->addWidget(m_pButtonMax);
    layout->addWidget(m_pButtonRestore);
    layout->addWidget(m_pButtonClose);

    layout->setContentsMargins(5, 0, 5, 0);
    layout->setSpacing(3);

    this->setFixedHeight(TITLE_HEIGHT);
    this->setFixedWidth(100);
    this->setWindowFlags(Qt::FramelessWindowHint);

    //加载css
    QFile file(":/Resources/title/title.css");
    file.open(QFile::ReadOnly);
    if (file.isOpen())
    {
        QString styleSheet = file.readAll();
        this->setStyleSheet(styleSheet);
    }
}

// 保存窗口最大化前窗口的位置以及大小;
void TitleBar::saveRestoreInfo(const QPoint point, const QSize size)
{
    m_restorePos = point;
    m_restoreSize = size;
}

// 获取窗口最大化前窗口的位置以及大小;
void TitleBar::getRestoreInfo(QPoint& point, QSize& size)
{
    point = m_restorePos;
    size = m_restoreSize;
}

void TitleBar::display_setting(bool display)
{
    if(display)
    {
        setting_display = true;
    }
    else
    {
        setting_display = false;
    }
    m_pButtonSetting->setVisible(setting_display);
}

void TitleBar::show_windows()
{
    label_version->setObjectName("label_version");
    label_version->setStyleSheet("#search_ico{background:transparent;}");
    QImage img(":/Resources/icon/OperatingSystem_32x32.png");
    label_version->setPixmap(QPixmap::fromImage(img));
    label_version->resize(16,16);
    label_version->setScaledContents(true);
    label_version->move(10,10);
    label_version->show();

    label_version->setVisible(true);
}

void TitleBar::setTitleContent(QString title)
{
    m_pTitle->setText(title);
}

void TitleBar::set_width(int width)
{
    this->title_width = width-18;
}
void TitleBar::initConnections()
{
    connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));     // 最小化
    connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked())); // 关闭
    connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));     // 最大化
    connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked())); //还原
}

void TitleBar::onButtonMinClicked()
{
    emit signalButtonMinClicked();
}

void TitleBar::onButtonCloseClicked()
{
    emit signalButtonCloseClicked();
}

void TitleBar::onButtonRestoreClicked()
{
    m_pButtonRestore->setVisible(false);
    m_pButtonMax->setVisible(true);
    emit signalButtonRestoreClicked();
}


void TitleBar::resize()
{
    this->set_width(this->parentWidget()->width());
    update();
}

void TitleBar::onButtonMaxClicked()
{
    m_pButtonMax->setVisible(false);
    m_pButtonRestore->setVisible(true);
    emit signalButtonMaxClicked();
}

void TitleBar::set_bg_color(int r,int g,int b)
{
    m_colorR = r;
    m_colorG = g;
    m_colorB = b;
    update();
}

//设置标题
void TitleBar::setTitle(QString title)
{
    m_pTitle->setText(title);
}

void TitleBar::setCenterTitle(QString title, QString color)
{
    center_title = new QLabel(this);
    center_title->setObjectName("center_title");
    center_title->setText(title);
    center_title->setStyleSheet("#center_title{color:"+color+";font-size:14px;}");
    center_title->resize(15*title.length(),25);
    center_title->move( (this->title_width - center_title->width())/2, 4);
}

// 绘制标题栏背景色;
void TitleBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPainterPath pathBack;
    pathBack.setFillRule(Qt::WindingFill);
    pathBack.addRoundedRect(QRect(0, 0, this->title_width, TITLE_HEIGHT), 0, 0);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathBack, QBrush(QColor(m_colorR, m_colorG, m_colorB)));
    this->setFixedWidth(this->title_width);
    QWidget::paintEvent(event);
}

void TitleBar::setButtonType(ButtonType buttonType)
{
    m_buttonType = buttonType;
    switch (buttonType)
    {
    case ONLY_CLOSE_BUTTON:
        m_pButtonMin->setVisible(false);
        break;
    case MIN_MAX_BUTTON:
        m_pButtonMax->setVisible(true);
        m_pButtonMin->setVisible(true);
        break;
    default:
        break;
    }
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit signalDbClicked();
}

// 以下通过mousePressEvent、mouseMoveEvent、mouseReleaseEvent三个事件实现了鼠标拖动标题栏移动窗口的效果;
void TitleBar::mousePressEvent(QMouseEvent *event)
{
    m_isPressed = true;
    m_startMovePos = event->globalPos();
    return QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPressed)
    {
        QPoint movePoint = event->globalPos() - m_startMovePos;
        QPoint widgetPos = this->parentWidget()->pos();
        m_startMovePos = event->globalPos();
        this->parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
    }
    return QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    return QWidget::mouseReleaseEvent(event);
}

TitleBar::~TitleBar()
{

}
