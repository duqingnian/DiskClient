#ifndef TITLEBAR_H
#define TITLEBAR_H

#include "Label.h"

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPaintEvent>
#include <QPushButton>
#include <QMouseEvent>

enum ButtonType
{
    MIN_BUTTON = 0,   // 最小化和关闭按钮;
    MIN_MAX_BUTTON,   // 最小化、最大化和关闭按钮;
    ONLY_CLOSE_BUTTON // 只有关闭按钮;
};

class TitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar();
    void initControl();
    void paintEvent(QPaintEvent *);
    void setTitle(QString title);
    void setCenterTitle(QString title,QString color);
    void set_bg_color(int,int,int);
    void initConnections();
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void set_width(int);
    void setTitleContent(QString);
    void setButtonType(ButtonType buttonType);
    void saveRestoreInfo(const QPoint point, const QSize size);
    void getRestoreInfo(QPoint& point, QSize& size);
    void display_setting(bool);
    QPushButton* m_pButtonSetting;      // 设置按钮
    void show_windows();
signals:
    void signalButtonMinClicked();
    void signalButtonCloseClicked();
    void signalButtonMaxClicked();
    void signalButtonRestoreClicked();
    void signalDbClicked();
public slots:
    void onButtonMinClicked();
    void onButtonCloseClicked();
    void onButtonMaxClicked();
    void onButtonRestoreClicked();
    void resize();
public:
    QLabel* m_pIcon;   // 标题栏图标
    QLabel* m_pTitle;  // 标题
    int m_colorR;
    int m_colorG;
    int m_colorB;
    int m_windowBorderWidth;

    bool setting_display = false;

    QPushButton* m_pButtonMin;			// 最小化按钮;
    QPushButton* m_pButtonMax;			// 最大化按钮;
    QPushButton* m_pButtonRestore;		// 最大化还原按钮;
    QPushButton* m_pButtonClose;		// 关闭按钮;
    // 移动窗口的变量;
    QPoint m_restorePos;
    QSize m_restoreSize;
    bool m_isPressed;
    QPoint m_startMovePos;
    int title_width; //窗体宽度
    ButtonType m_buttonType;
    Label* label_version; //版本按钮
    QLabel* center_title;
};

#endif // TITLEBAR_H
