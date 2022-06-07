#ifndef WATCHERTHREAD_H
#define WATCHERTHREAD_H

#include <QThread>
#include <QAtomicInt>
#include <windows.h>

class WatcherThread : public QThread
{
    Q_OBJECT
public:
    explicit WatcherThread(QString usr_dir,QObject *parent = nullptr);
    ~WatcherThread();
    void stop();
protected:
    void run();
    void watchChanges(size_t fileNotifyBufferSize, bool *increaseBufferSize);
    void closeHandle();
    void notifi_change(const QString &path, FILE_NOTIFY_INFORMATION * curEntry);
signals:
    void changed(const QString &path);
    void lostChanges();
    void ready();
private:
    QString _path;
    HANDLE _directory;
    HANDLE _resultEvent;
    HANDLE _stopEvent;
    QAtomicInt _done;
    QString watch_path;
    FILE_NOTIFY_INFORMATION* pFileNotifyBuffer;
};

#endif // WATCHERTHREAD_H
