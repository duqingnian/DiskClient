#ifndef UPLOADDIRTHREAD_H
#define UPLOADDIRTHREAD_H

#include <QTcpSocket>
#include <QThread>

class UploadDirThread : public QThread
{
    Q_OBJECT
public:
    explicit UploadDirThread(QObject *parent = nullptr);
    void set_dir(QString);
    void set_socket_descriptor(qintptr);
    void run() override;
    void loop_dir(QString,bool sub=true);
    QString UploadDirThread::md5(QString str);
private:
    QString dir;
    QTcpSocket* socket;
    qintptr _descriptor;
};

#endif // UPLOADDIRTHREAD_H
