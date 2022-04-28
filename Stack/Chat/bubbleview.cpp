#include "bubbleview.h"
#include <QDebug>

BubbleView::BubbleView(QWidget *parent,BubbleInfo *info) : QWidget(parent),info(info)
{
    textFont = QFont("Microsoft YaHei", 10, 50, false);
    sysMsgFont = QFont("Microsoft YaHei", 9, 50, false);
    setFont(textFont);

    QStringList tmp;

    tmp << info->msg_sendor->avatar << "" << "";
    headIcon = new AdoButton(this,tmp,QSize(40,40),AVATAR);
    headIcon->id = info->msg_sendor->job_number;
    headIcon->setObjectName("avatar_"+info->msg_sendor->job_number);
    headIcon->name = info->msg_sendor->name;
    headIcon->head = info->msg_sendor->avatar;
    headIcon->setStyleSheet("#avatar_"+info->msg_sendor->job_number+"{background:red;}");
}

QSize BubbleView::setRect()
{
    int Blank1 = 10;//头像和气泡框之间的空白宽度
    int Blank2 = 20;//头像和边框之间的空白宽度
    int Blank3 = 20;//气泡框顶部和头像顶部的空白宽度
    int Blank4 = 10;//气泡边框和内边矩形的宽度
    int headIconWH = 40;

    iconRect = QRect(this->geometry().width() - Blank2 - headIconWH,Blank2,headIconWH,headIconWH);
    triangleRect = QRect(iconRect.x()-Blank1,iconRect.y()+Blank3+10,5,10);

    QSize textSize = getRealString(info->msg);
    frameRect = QRect(iconRect.x()-Blank1-Blank4*2 - textSize.width(),iconRect.y()+Blank3,
                      textSize.width() + Blank4*2,textSize.height() + Blank4*2);
    textRect = QRect(frameRect.x() + Blank4,frameRect.y()+Blank4,textSize.width(),textSize.height());
    height = frameRect.height() + (Blank2+Blank3);

    return QSize(this->width(),height);
}

QSize BubbleView::getRealString(QString src)
{
    QFont font = this->font();

    QFontMetrics fm(font);

    lineHeight = fm.lineSpacing();
    int cnt = src.count("\n");
    int maxWidth = 0;
    textWidth = this->width() - 150;


    if(cnt == 0){
        maxWidth = fm.width(src);
        QString value = src;
        if(maxWidth > textWidth){
            maxWidth = textWidth;
            int size = textWidth/fm.width(" ");
            int num = fm.width(value)/textWidth;
            num = (fm.width(value))/textWidth;
            cnt += num;
            QString tmp = "";
            for(int i = 0;i < num;i++){
                tmp += value.mid(i*size,(i+1)*size) + "\n";
            }
            //src.replace(value,tmp);
        }
    }else{
        for(int i = 0;i < cnt + 1;i++){
            QString value = src.split("\n").at(i);
            maxWidth = fm.width(value) > maxWidth ? fm.width(value) : maxWidth;
            if(fm.width(value) > textWidth){
                maxWidth = textWidth;
                int size = textWidth/fm.width(" ");
                int num = fm.width(value)/textWidth;
                num = ((i+num)*fm.width(" ") + fm.width(value))/textWidth;
                cnt += num;
                QString tmp = "";
                for(int j = 0;j < num;j++){
                    tmp += value.mid(i*size,(i+1)*size) + "\n";
                }
                //src.replace(value,tmp);
            }
        }
    }
    return QSize(maxWidth,(cnt+1)*lineHeight);
}

void BubbleView::paintEvent(QPaintEvent *)
{
    qDebug() << "paintEvent...";
    QPainter painter(this);
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::gray));

    headIcon->setGeometry(iconRect.x(),iconRect.y(),45,45);
    QColor color_frame("#abcee5");
    painter.setBrush(QBrush(color_frame));
    painter.drawRoundedRect(frameRect,13,13);

//    QPointF points[3] = {
//        QPointF(triangleRect.x(),triangleRect.y()),
//        QPointF(triangleRect.x()+triangleRect.width(),triangleRect.y()+triangleRect.height()/2),
//        QPointF(triangleRect.x(),triangleRect.y()+triangleRect.height())
//    };
//    QColor color_frame2("#abcee5");
//    QPen pen;
//    pen.setColor(color_frame2);
//    painter.setPen(pen);
//    painter.drawPolygon(points,3);

//    loadingLabel->setGeometry(frameRect.x()-20,frameRect.y() + frameRect.height()/2 - 8,16,16);

//    //画文本内容
//    QPen pen_text;
//    pen_text.setColor(Qt::black);
//    painter.setPen(pen_text);
//    QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
//    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
//    painter.setFont(this->font());
//    painter.drawText(textRect,info->msg,option);
}














