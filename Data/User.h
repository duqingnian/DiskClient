#ifndef USER_H
#define USER_H

#include <QMetaType>
#include <QString>

struct USER
{
    QString uid;
    QString name;
    QString depid;
    QString depname;
    QString groupid;
    QString groupname;
    QString title;
    QString job_number;
};

Q_DECLARE_METATYPE(USER);

#endif // USER_H
