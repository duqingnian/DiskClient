#ifndef DOWNLOADTHREAD_H
#define DOWNLOADTHREAD_H

#include <QThread>

class DownLoadThread : public QThread
{
    Q_OBJECT
public:
    explicit DownLoadThread(QString _url,QString _savePath,QObject *parent = nullptr);
    void run();

private:
    QString url;
    QString savePath;
};

#endif // DOWNLOADTHREAD_H
