#ifndef FD_H
#define FD_H

#include <QMetaType>
#include <QString>

struct FD
{
    int id = 0;
    QString name = "";
    QString suffix = "";
    QString icon = "";
    int size = 0;
    int created_at = 0;
    QString created_time = "";
    int updated_at = 0;
    QString updated_time = "";
    bool selected = false;
};

Q_DECLARE_METATYPE(FD);


#endif // FD_H
