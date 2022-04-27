#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QTcpSocket>

class Socket : public QObject
{
    Q_OBJECT
public:
    static Socket* Instance();
    QString getErr();
    bool isOpen();
    bool send(QString header,QString msg);
private:
    QString err = "";
    QTcpSocket* client;
    explicit Socket(QObject *parent = nullptr);
signals:

};

#endif // SOCKET_H
