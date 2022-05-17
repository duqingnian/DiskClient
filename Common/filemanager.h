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
    void set_file(UP_FILE*);
    void set_socket_descriptor(qintptr);
    void start_upload();
    void wait( int ms );
    void run() override;
private:
    UP_FILE* file;
    QTcpSocket* socket;
    qintptr _descriptor;

signals:


};

#endif // FILEMANAGER_H
