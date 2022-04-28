#ifndef ADOBUTTON_H
#define ADOBUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QStringList>
#include <QMouseEvent>
#include <QEvent>
#include <QSize>

enum BTNTYPE{
    NORMAL,  //普通按钮
    AVATAR     //头像按钮
};

class AdoButton : public QPushButton
{
    Q_OBJECT
public:

    explicit AdoButton(QWidget *parent,QStringList iconsList,QSize size,BTNTYPE type = NORMAL);
    void setImage(QString);
    void onBtnClicked();
    void restoreBtn();

    void changeIconSet(QStringList);

    QString NormalIcon;//正常显示时的图片
    QString MoveInIcon;//光标移入时的图片
    QString ClickIcon;//点击时的光标
    QSize btnSize;
    BTNTYPE btnType;
    bool tag = false;

    //弹出个人信息界面中使用
    QString id;
    QString name;
    QString head;

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *event);
};

#endif // ADOBUTTON_H
