#pragma execution_character_set("utf-8")
#include "db.h"

#include <QCoreApplication>
#include <QMutex>
#include <QDebug>
#include <QMessageBox>

Db::Db(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("123456");
    db.setDatabaseName("disk");

    if (!db.open())
    {
        QString err = "无法打开数据库";
        QMessageBox msgBox;
        msgBox.setText(err);
        msgBox.exec();
    }
    else
    {
        qDebug() << "数据库打开成功";
    }
}

Db *Db::Instance()
{
    static QMutex mutex;
    static QScopedPointer<Db> inst;
    if (Q_UNLIKELY(!inst)) {
        mutex.lock();
        if (!inst) {
            inst.reset(new Db);
        }
        mutex.unlock();
    }
    return inst.data();
}

bool Db::isOpen()
{
    if (!db.open())
    {
        qDebug() << "数据库打开失败! err:" << db.lastError();
        return false;
    }
    return true;
}

void Db::query(QString sql)
{
    QSqlQuery sql_query;
    sql_query.exec(sql);
}
