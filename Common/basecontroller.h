#ifndef BASECONTROLLER_H
#define BASECONTROLLER_H

#include <QObject>
#include <QDialog>
#include <Data/User.h>
#include <Data/TabMeta.h>
#include <Data/Simple.h>
#include <Data/FD.h>
#include <QSettings>
#include <QTcpSocket>

extern USER     *user;
extern TABMETA  *tab_meta;
extern SIMPLE   *simple;
extern FD       *fd;

class BaseController : public QDialog
{
    Q_OBJECT
public:
    explicit BaseController(QWidget *parent = nullptr);
    ~BaseController();

    int border_width = 9;
    QString md5(QString);
    QString random(int);
    int get_time();

    QSettings* regedit;

    QTcpSocket* socket;
    QString server_ip;
    QString server_port;

    //根据键值获取注册表的属性值
    QString get_reg(QString);
    //生成接口地址
    QString path(QString);
    //解析简单的json
    SIMPLE SimpleParse(QString);
    //是不是汉字
    bool is_cn(QChar);

    void box(QString);

    QString GetStrByQJsonObject(QJsonObject);

    ////////////////////////////////////
    // 客户端发送消息到服务器
    ////////////////////////////////////
    bool send(QString,QString);
    bool sendJsonObject(QString,QJsonObject);


signals:

};

#endif // BASECONTROLLER_H
