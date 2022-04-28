#include "adobutton.h"
#include "Common/adohelper.h"


#include <QPixmap>
#include <QBitmap>
#include <QDebug>
#include <QPainter>
#include <QPen>

AdoButton::AdoButton(QWidget *parent,QStringList iconsList,QSize Iconsize,BTNTYPE type):
    QPushButton(parent)
{
    if(iconsList.size() < 3){
        qDebug() << "AdoButton.cpp : fatal error!icon number is too few";
    }
    NormalIcon = iconsList.at(0);
    MoveInIcon = iconsList.at(1);
    ClickIcon = iconsList.at(2);
    btnSize = Iconsize;
    btnType = type;

    setFixedSize(btnSize);
    setImage(NormalIcon);
}

void AdoButton::enterEvent(QEvent *e)
{
    if(btnType == NORMAL){
        if(!tag){
            setImage(MoveInIcon);
            setCursor(Qt::PointingHandCursor);
        }
    }else{
        setCursor(Qt::PointingHandCursor);
        //setStyleSheet("border:2px solid white;border-radius:25px;");
    }

    return QPushButton::enterEvent(e);
}

void AdoButton::leaveEvent(QEvent *e)
{
    if(btnType == NORMAL){
        if(!tag){
            setImage(NormalIcon);
            setCursor(Qt::ArrowCursor);
        }
    }else{
        setCursor(Qt::ArrowCursor);
        //resize(btnSize);
    }

    return QPushButton::leaveEvent(e);
}

void AdoButton::paintEvent(QPaintEvent *event)
{
    return QPushButton::paintEvent(event);
}


#if 0
void LeftBarButton::mousePressEvent(QMouseEvent *)
{
    setIcon(ClickIcon);
    qDebug() << "mouse press:" << this->size();
}

void LeftBarButton::mouseReleaseEvent(QMouseEvent *)
{
    setIcon(NormalIcon);
    qDebug() << "mouse release:" << this->size();
}

#endif

void AdoButton::setImage(QString iconName)
{
    if(btnType == AVATAR){
        QPixmap pixmap = AdoHelper::PixmapToRound(QPixmap(iconName),btnSize.width()/2);
//        QPixmap pixmap = myHelper::PixmapToRound(
//                    QPixmap("E:\\100001.png"),btnSize.width()/2);
        setIcon(QIcon(pixmap));
        setIconSize(size());
        setStyleSheet("border:none;");
    }else{
        setIcon(QIcon(iconName));
        setIconSize(size());
        setStyleSheet("border:none;");
    }
}

void AdoButton::onBtnClicked()
{
    if(btnType == NORMAL){
        setImage(ClickIcon);
        tag = true;
    }
}

void AdoButton::restoreBtn()
{
    if(btnType == NORMAL){
        setImage(NormalIcon);
        tag = false;
    }
}

void AdoButton::changeIconSet(QStringList iconsList)
{
    NormalIcon = iconsList.at(0);
    MoveInIcon = iconsList.at(1);
    ClickIcon = iconsList.at(2);
}

