#ifndef MENUITEM_H
#define MENUITEM_H

#include <QMetaType>
#include <QString>

struct MenuItem
{
    QString text;
    QString key;
    QString ico;
    QString color;
    QString border;
    int id = 0;
};

Q_DECLARE_METATYPE(MenuItem);

#endif // MENUITEM_H
