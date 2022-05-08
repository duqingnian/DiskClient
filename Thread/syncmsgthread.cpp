#include "syncmsgthread.h"
#include <QDebug>

SyncMsgThread::SyncMsgThread(QString _job_number,QObject *parent) : QThread(parent)
{
    job_number = _job_number;
}

void SyncMsgThread::run()
{
    for(int i=0;i<100;i++)
    {
        QThread::sleep(1);

        emit sync_msg("["+QString::number(i)+"]" + job_number);
    }
}
