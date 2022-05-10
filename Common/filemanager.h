#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QTcpSocket>

#include <Data/UploadFileUnit.h>

class FileManager : public QObject
{
    Q_OBJECT
public:
    static FileManager* Instance();
    QString getErr();
    bool isOpen();
    bool upload(QString md5,UP_FILE* file);
    QTcpSocket* handle();
    void disconnect();
    void wait( int ms );
private:
    QString err = "";
    QTcpSocket* socket;
    explicit FileManager(QObject *parent = nullptr);
signals:


};

#endif // FILEMANAGER_H
