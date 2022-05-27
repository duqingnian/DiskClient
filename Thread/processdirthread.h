#ifndef PROCESSDIRTHREAD_H
#define PROCESSDIRTHREAD_H

#include <QFileInfo>
#include <QThread>

class ProcessDirThread : public QThread
{
    Q_OBJECT
public:
    explicit ProcessDirThread(QObject *parent = nullptr);
    void set_dir(QString);
    void loop_dir(QString,bool sub=true);
    void run() override;
private:
    QString dir;
signals:
    void find_file(QString,QString);
    void sync_size(unsigned long long);
    void process_complete();
};

#endif // PROCESSDIRTHREAD_H
