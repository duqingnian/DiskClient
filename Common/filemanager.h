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
    void set_file(QString meta_key,int meta_id,int fd_id,UP_FILE*);
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
signals:


};

#endif // FILEMANAGER_H
