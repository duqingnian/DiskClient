#ifndef TABMETA_H
#define TABMETA_H

#include <QMetaType>
#include <QString>

struct TABMETA
{
    QString objname;
    QString txt;
    int width=65;
    QString bgcolor="";
};

Q_DECLARE_METATYPE(TABMETA);


#endif
