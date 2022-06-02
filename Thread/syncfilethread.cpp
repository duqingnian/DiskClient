#include "syncfilethread.h"

#include <QDataStream>

SyncFileThread::SyncFileThread(QObject *parent) : QThread(parent)
{
    socket = nullptr;
}

void SyncFileThread::set_socket_descriptor(qintptr desc)
{
    this->_descriptor = desc;
}

void SyncFileThread::run()
{
    qDebug() << "SyncFileThread.....";
    if(socket == nullptr)
    {
        socket = new QTcpSocket();
        if(!socket->setSocketDescriptor(this->_descriptor))
        {
            qDebug() << "SyncFileThread::run setSocketDescriptor error !!!";
            return;
        }
    }
    if(socket->isOpen())
    {
        qDebug() << "SyncFileThread OK ........";
        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
        connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    }
    else
    {
        qDebug() << "SyncFileThread::is not open";
    }
    qDebug() << "running........";
    for(int i=0;i<20;i++)
    {
        QThread::sleep(1);
        qDebug() << "x================================" << i;
    }
    exec();
}

void SyncFileThread::readyRead()
{
    QByteArray buffer;

    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_15);
    socketStream.startTransaction();
    socketStream >> buffer;
    socket->flush();

    if(!socketStream.commitTransaction())
    {
        return;
    }
    qDebug() << "buffer=" << buffer;
//    QByteArray data = buffer.mid(256);
//    QByteArray header = buffer.mid(0,buffer.indexOf(";"));

//    if("DQN:?" != header.mid(0,5))
//    {
//        return;
//    }

//    //QString _header = header;

//    qDebug() << "Sync=" << header;
}

void SyncFileThread::disconnected()
{

}
