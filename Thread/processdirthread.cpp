#pragma execution_character_set("utf-8")
#include "processdirthread.h"
#include <QDir>
#include <QDebug>

ProcessDirThread::ProcessDirThread(QObject *parent) : QThread(parent)
{

}

void ProcessDirThread::set_dir(QString _dir)
{
    this->dir = _dir;
}

void ProcessDirThread::loop_dir(QString path, bool _sub)
{
    QDir dir(path);
    if(!dir.exists())
        return;
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);    //过滤后只留下文件夹、文件以及隐藏文件，不包括当前文件夹和上级文件夹
    dir.setSorting(QDir::DirsFirst);    //优先排列文件夹

    foreach(QFileInfo info,dir.entryInfoList())
    {
        QThread::usleep(1);
        if(info.isFile())
        {
            emit find_file("FILE",info.absoluteFilePath());
            emit sync_size(info.size());
        }
        else
        {
            emit find_file("DIR",info.absolutePath());
            loop_dir(info.filePath(),false);
        }
    }
    if(_sub)
    {
        emit process_complete();
    }
}

void ProcessDirThread::run()
{
    this->loop_dir(this->dir,true);
}
