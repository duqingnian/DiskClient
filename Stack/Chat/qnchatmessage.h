﻿#ifndef QNCHATMESSAGE_H
#define QNCHATMESSAGE_H

#include <QWidget>

#include <Common/basecontroller.h>

class QPaintEvent;
class QPainter;
class QLabel;
class QMovie;

class QNChatMessage : public BaseController
{
    Q_OBJECT
public:
    explicit QNChatMessage(SELECT_UNIT*,QWidget *parent = nullptr);

    enum User_Type{
        User_System,//系统
        User_Me,    //自己
        User_She,   //用户
        User_Time,  //时间
    };
    void setTextSuccess();
    void setText(QString text, QString time, QSize allSize, User_Type userType);

    QSize getRealString(QString src);
    QSize fontRect(QString str);

    inline QString text() {return m_msg;}
    inline QString time() {return m_time;}
    inline User_Type userType() {return m_userType;}
protected:
    void paintEvent(QPaintEvent *event);
private:
    SELECT_UNIT* target;
    QString m_msg;
    QString m_time;
    QString m_curTime;

    QSize m_allSize;
    User_Type m_userType = User_System;

    int m_kuangWidth;
    int m_textWidth;
    int m_spaceWid;
    int m_lineHeight;

    QRect m_iconLeftRect;
    QRect m_iconRightRect;
    QRect m_sanjiaoLeftRect;
    QRect m_sanjiaoRightRect;
    QRect m_kuangLeftRect;
    QRect m_kuangRightRect;
    QRect m_textLeftRect;
    QRect m_textRightRect;
    QPixmap m_leftPixmap;
    QPixmap m_rightPixmap;
    QLabel* m_loading = Q_NULLPTR;
    QMovie* m_loadingMovie = Q_NULLPTR;
    bool m_isSending = true;
};

#endif // QNCHATMESSAGE_H
