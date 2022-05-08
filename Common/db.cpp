#pragma execution_character_set("utf-8")
#include "db.h"

#include <QCoreApplication>
#include <QMutex>
#include <QDebug>
#include <QMessageBox>

Db::Db(QObject *parent) : QObject(parent)
{
    qDebug() << QSqlDatabase::drivers();

    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("123456");
    db.setDatabaseName("disk");

    if (!db.open())
    {
        QString err = "Error: Failed to connect database.";
        QMessageBox msgBox;
        msgBox.setText(err);
        msgBox.exec();
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
    if (!db.open())//判断数据库是否打开
    {
        qDebug() << "db open faild! err:" << db.lastError();
        return false;
    }
    qDebug() << "数据库打开成功";
    return true;
}

void Db::query(QString sql)
{
    QSqlQuery sql_query;
    sql_query.exec(sql);
}
