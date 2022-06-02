#ifndef SYNCFILETHREAD_H
#define SYNCFILETHREAD_H

#include <QTcpSocket>
#include <QThread>

class SyncFileThread : public QThread
{
    Q_OBJECT
public:
    explicit SyncFileThread(QObject *parent = nullptr);
    void set_socket_descriptor(qintptr);
    void run() override;
private:
    QTcpSocket* socket;
    qintptr _descriptor;


private slots:
    void readyRead();
    void disconnected();
};

#endif // SYNCFILETHREAD_H
