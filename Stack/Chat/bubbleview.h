#ifndef BUBBLEVIEW_H
#define BUBBLEVIEW_H

#include <QLabel>
#include <QWidget>
#include "bubbleinfo.h"
#include <QPainter>
#include <QPaintEvent>
#include <Component/adobutton.h>

class BubbleView : public QWidget
{
    Q_OBJECT
public:
    explicit BubbleView(QWidget *parent = nullptr,BubbleInfo* info = nullptr);
    QSize setRect();
    QSize getRealString(QString src);

    BubbleInfo *info;
private:

    int height;//整个widget的高度
    int textWidth;
    int lineHeight;

    QFont textFont;
    QFont sysMsgFont;//系统消息字体

    QRect iconRect;
    QRect triangleRect;//气泡中的小三角
    QRect frameRect;
    QRect textRect;

    AdoButton* headIcon;
    QLabel *loadingLabel;
    QMovie *loadingMovie;

    QLabel *fileNameLabel;
    QLabel *fileIconLabel;
    QLabel *fileSizeLabel;
    QLabel *picture;
    QLabel *downloadLabel;

    QLabel *errorLabel;

    //QMenu *popMenu;
protected:
    void paintEvent(QPaintEvent*);
    //void mousePressEvent(QMouseEvent *);
    //void enterEvent(QEvent *);
    //void leaveEvent(QEvent *);
};

#endif // BUBBLEVIEW_H
