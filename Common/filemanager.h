#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <Data/UploadFileUnit.h>

class FileManager : public QThread
{
    Q_OBJECT
public:
    explicit FileManager(QObject *parent = 0);
    bool isOpen();
    void set_file(QString meta_key,int meta_id,int fd_id,QString,UP_FILE*);
    void set_socket_descriptor(qintptr);
    void start_upload();
    void wait( int ms );
    void run() override;

private:
    UP_FILE* file;
    QTcpSocket* socket;
    qintptr _descriptor;

    QString META_KEY;
    int META_ID;
    int FD_ID;
    QString SJN="";

    uint start_time = 0;
    unsigned long long SPEED = 0;

private slots:
    void readyRead();
    void disconnected();
signals:
    void update_progress(QString BUNDLE,QString BUNDLE_ID,QString FD_ID,QString md5, QString state, float pct,unsigned long long SPEED);
};

#endif // FILEMANAGER_H
