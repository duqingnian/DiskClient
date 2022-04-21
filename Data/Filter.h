#ifndef FILTER_H
#define FILTER_H

#include <QString>


struct FILE_FILTER
{
    QString objname;
    QString txt;
    QString ico = "";
    bool cate = false;
    QString css = "";
};

#endif // FILTER_H
