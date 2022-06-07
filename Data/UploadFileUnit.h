#ifndef UPLOADFILEUNIT_H
#define UPLOADFILEUNIT_H

#include <QString>

enum UP_STATE{
    UPLOAD, //上传
    UPDATE //更新
};

struct UP_FILE{
    int id = 0;
    QString md5 = "";
    QString suffix = "";
    QString ico = "";
    QString name = "";
    QString path = "";
    unsigned long long size = 0;
    UP_STATE state;

};
#endif // UPLOADFILEUNIT_H
