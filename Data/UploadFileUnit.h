#ifndef UPLOADFILEUNIT_H
#define UPLOADFILEUNIT_H

#include <QString>

enum UP_STATE{
    WAIT_UP, //准备上传
    UP_ING,  //上传中
    UP_FAILED //上传失败
};

struct UP_FILE{
    QString md5 = "";
    QString suffix = "";
    QString ico = "";
    QString name = "";
    QString path = "";
    unsigned long long size = 0;
    UP_STATE state;

};
#endif // UPLOADFILEUNIT_H
