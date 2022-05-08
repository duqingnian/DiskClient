#ifndef DB_H
#define DB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>

class Db : public QObject
{
    Q_OBJECT
public:
    explicit Db(QObject *parent = nullptr);
    static Db* Instance();
    QSqlDatabase db;
    bool isOpen();
    void query(QString sql);
signals:

};

#endif // DB_H
