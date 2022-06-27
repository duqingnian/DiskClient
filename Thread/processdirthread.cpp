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
    {
        return;
    }
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);    //优先排列文件夹

    foreach(QFileInfo info,dir.entryInfoList())
    {
        QThread::usleep(5);
        if(info.isDir())
        {
            //QThread::usleep(5);
            dir_count++;
            emit find_file("DIR",info.filePath());
            loop_dir(info.filePath(),false);
        }
        else
        {
            file_count++;
            emit find_file("FILE",info.absoluteFilePath());
            emit sync_size(info.size());
            //QThread::usleep(5);
        }
    }
    if(_sub)
    {
        //qDebug() << "文件夹扫描完成";
        emit process_complete();
    }
}

void ProcessDirThread::run()
{
    this->loop_dir(this->dir,true);
}
