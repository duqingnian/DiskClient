#ifndef BASECONTROLLER_H
#define BASECONTROLLER_H

#include <QObject>
#include <QDialog>
#include <Data/User.h>
#include <Data/TabMeta.h>
#include <Data/Simple.h>
#include <Data/FD.h>
#include <QMutex>
#include <QSettings>
#include <QTcpSocket>
#include <QFile>
#include "socket.h"
#include "db.h"

extern USER     *user;
extern TABMETA  *tab_meta;
extern SIMPLE   *simple;
extern FD       *fd;

struct SELECT_UNIT{
    int index = 0;
    QString job_number = "";
    USER* unit;
};

class BaseController : public QDialog
{
    Q_OBJECT
public:
    explicit BaseController(QWidget *parent = nullptr);
    ~BaseController();

    int border_width = 8;
    QString md5(QString);
    QString random(int);
    int get_time();
    QString get_mtime();

    QSettings* regedit;

    QString server_ip;
    QString server_port;

    QString basepath = "";

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

    bool sendmsg(QString sendto,QString msg="",QString header="");
    bool sendJsonObject(QString sendto,QJsonObject,QString header="");

    //计算文件的md5
    QString md5_file(const QString &sourceFilePath);

    //获取用户头像
    QPixmap GetAvatar();
    QPixmap GetAvatar(QString);

signals:

};

#endif // BASECONTROLLER_H
