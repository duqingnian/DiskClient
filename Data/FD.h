#ifndef FD_H
#define FD_H

#include <QMetaType>
#include <QString>

struct FD
{
    int id = 0;
    QString md5 = "";
    QString name = "";
    QString suffix = "";
    QString icon = "";
    int size = 0;
    int created_at = 0;
    int updated_at = 0;
    QString version = "0";
    bool selected = false;
    QString creator = "";
};

Q_DECLARE_METATYPE(FD);


#endif // FD_H
