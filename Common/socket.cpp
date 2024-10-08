﻿#include "socket.h"

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
        client->abort();
        if(!client->isOpen())
        {
            client->connectToHost(server_ip,server_port.toInt());
            if(!client->waitForConnected())
            {
                err = QString("socket连接失败, err:%1.").arg(client->errorString());
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
    msg = "ADO"+_header+"#"+msg;
    int len = client->write(msg.toUtf8());
    if(len > 0)
    {
        return true;
    }
    return false;
}

QTcpSocket *Socket::handle()
{
    return client;
}

void Socket::disconnect()
{
    if(client && client->isOpen())
    {
        client->disconnect();
    }
    client->deleteLater();
}






