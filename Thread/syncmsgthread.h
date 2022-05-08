#ifndef SYNCMSGTHREAD_H
#define SYNCMSGTHREAD_H

#include <QThread>

class SyncMsgThread : public QThread
{
    Q_OBJECT
public:
    explicit SyncMsgThread(QString job_number,QObject *parent = nullptr);
    void run();

private:
    QString job_number;

signals:
    void sync_msg(QString);
};

#endif // SYNCMSGTHREAD_H
