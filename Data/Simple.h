#ifndef SIMPLE_H
#define SIMPLE_H

#include <QMetaType>
#include <QString>

struct SIMPLE
{
    QString title;
    QString id;
};

Q_DECLARE_METATYPE(SIMPLE);


#endif
