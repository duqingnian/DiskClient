#pragma execution_character_set("utf-8")
#include "downloadthread.h"
#include <QDebug>
#include <Lib/HttpClient.h>

DownLoadThread::DownLoadThread(QString _url,QString _savePath,QObject *parent)
{
    this->url = _url;
    this->savePath = _savePath;
}

void DownLoadThread::run()
{
}
