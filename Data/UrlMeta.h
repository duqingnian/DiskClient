#ifndef URLMETA_H
#define URLMETA_H

#include <QMetaType>
#include <QString>

struct UrlMeta
{
    QString key;
    QString text;
    QString category;
    int id;
};

Q_DECLARE_METATYPE(UrlMeta);


#endif // URLMETA_H
