#include "socket.h"

#include <QDataStream>
#include <QMutex>
#include <QSettings>
#include <QDebug>

Socket* Socket::Instance()
{
    static QMutex mutex;
    static QScopedPointer<Socket> inst;
    if (Q_UNLIKELY(!inst)) {
        mutex.lock();
        if (!inst) {
            inst.reset(new Socket);
        }
        mutex.unlock();
    }
    return inst.data();
}

Socket::Socket(QObject *parent) : QObject(parent)
{
    QSettings* regedit  = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\AdoDisk", QSettings::NativeFormat);
    QString server_ip   = regedit->value("socket_server").toString();
    QString server_port = regedit->value("socket_port").toString();

    if("" != server_ip && "" != server_port)
    {
        client = new QTcpSocket(this);
        if(!client->isOpen())
        {
            client->connectToHost(server_ip,server_port.toInt());
            if(client->waitForConnected())
            {
                qDebug() << "socket connect success";
                err = "";

            }
            else
            {
                err = QString("socket connect failed, err:%1.").arg(client->errorString());
                qDebug() << err; //"socket connect failed, err:Connection refused."
            }
        }
    }
}

QString Socket::getErr()
{
    return err;
}

bool Socket::isOpen()
{
    return client->isOpen();
}

bool Socket::send(QString _header, QString msg)
{
    if(!client)
    {
        return false;
    }
    if(!client->isOpen())
    {
        return false;
    }
    QDataStream socketStream(client);
    socketStream.setVersion(QDataStream::Qt_5_12);

    QByteArray header;
    header.prepend(_header.toUtf8());
    header.resize(128);

    QByteArray byteArray = msg.toUtf8();
    byteArray.prepend(header);

    socketStream << byteArray;
    byteArray.clear();

    return true;
}






