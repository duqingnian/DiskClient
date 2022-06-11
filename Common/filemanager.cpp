#pragma execution_character_set("utf-8")
#include "filemanager.h"
#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QThread>

#define DATA_META_LEN 256

//构造函数
FileManager::FileManager(QObject *parent) : QThread(parent)
{
    socket = nullptr;

    META_KEY = "";
    META_ID = 0;
    FD_ID = 0;
}

//设置文件
void FileManager::set_file(QString meta_key,int meta_id,int fd_id,QString _sjn,UP_FILE* _file)
{
    META_KEY = meta_key;
    META_ID = meta_id;
    FD_ID = fd_id;
    SJN = _sjn;
    this->file = _file;
}

//设置socket的描述符
void FileManager::set_socket_descriptor(qintptr descriptor)
{
    _descriptor = descriptor;
}

void FileManager::run()
{
    if(true)
    {
        if(socket == nullptr)
        {
            socket = new QTcpSocket();
            socket->setSocketDescriptor(_descriptor);

            connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
            connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
        }
        if(socket->isOpen())
        {
            QString file_location = file->path;
            QFile m_file(file_location);
            if(m_file.open(QIODevice::ReadOnly))
            {
                QDataStream socketStream(socket);
                socketStream.setVersion(QDataStream::Qt_5_15);

                int len = 0;
                unsigned long long left_size = m_file.size();

                int buf_size = 1024*64 - DATA_META_LEN;
                bool running = true;

                start_time = QDateTime::currentDateTime().toTime_t();

                do{
                    float PCT = 0.0f;
                    if(0 == left_size)
                    {
                        PCT = 100.00f;
                    }
                    else
                    {
                        PCT = static_cast<float>(file->size - left_size) / static_cast<float>(file->size);
                        PCT = PCT * 100;
                    }

                    uint t = QDateTime::currentDateTime().toTime_t();
                    if(t > start_time)
                    {
                        SPEED = static_cast<unsigned long long>(file->size - left_size) / static_cast<unsigned long long>(t - start_time);
                    }

                    QString meta = "ADOFILE:?";
                    meta += "FILE_ID:"+QString::number(file->id);
                    meta += ",MD5:"+file->md5;
                    meta += ",SUFFIX:"+file->suffix;
                    meta += ",LEFT_SIZE:"+QString::number(left_size);
                    meta += ",BUF_SIZE:"+QString::number(buf_size);
                    meta += ",FS:"+QString::number(m_file.size());
                    meta += ",PCT:"+QString::number(PCT);
                    meta += ",BUNDLE:"+META_KEY;
                    meta += ",BUNDLE_ID:"+QString::number(META_ID);
                    meta += ",FD_ID:"+QString::number(FD_ID);
                    meta += ",NAME:"+file->name;
                    meta += ",SJN:"+SJN;
                    meta += ";";

                    QByteArray data;
                    data.prepend(meta.toUtf8());
                    data.resize(DATA_META_LEN);

                    QByteArray buf_str;
                    buf_str = m_file.read(buf_size);

                    if(left_size < buf_size)
                    {
                        running = false;
                        len = buf_str.length();
                        buf_str.resize(buf_size);
                    }
                    else
                    {
                        len = buf_str.length();
                    }
                    len = buf_str.length();
                    data.append(buf_str);

                    socketStream << data;
                    socket->flush();

                    QThread::usleep(2);

                    if(left_size >= len)
                    {
                        left_size -= len;
                    }
                    else
                    {
                        left_size = 0;
                    }

                    socket->waitForBytesWritten();

                    if(!running)
                    {
                        PCT = 100.00f;
                        socket->flush();
                        socket->waitForBytesWritten();
                        data.clear();

                    }
                    emit update_progress(META_KEY,QString::number(META_ID),QString::number(FD_ID),file->md5, "UPLOADING", PCT,SPEED);
                }while(running);
            }
            else
            {
                qDebug() << "file opend failed!";
            }
        }
        else
        {
            qDebug() << "FileManager file socket is not open!";
        }

        exec();
    }
}

void FileManager::readyRead()
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

    QByteArray data = buffer.mid(DATA_META_LEN);
    QByteArray header = buffer.mid(0,buffer.indexOf(";"));

    if("DQN:?" != header.mid(0,5) || header.length() < 30)
    {
        return;
    }

    QString _header = header;
    if("DQN:?META:UPLOAD_COMPLETE" == _header.mid(0,25))
    {
        QString MD5 ="";
        QString LEFT_SIZE = "0";
        QString PCT = "";
        QString BUNDLE = "";
        QString BUNDLE_ID = "0";
        QString FD_ID = "0";

        QStringList headers = _header.split(",");
        for(int i=0;i<headers.length();i++)
        {
            QString hi = headers[i];
            QStringList his = hi.split(":");
            if("MD5" == his[0]){MD5 = his[1];}
            else if("LEFT_SIZE" == his[0]){LEFT_SIZE = his[1];}
            else if("PCT" == his[0]){PCT = his[1];}
            else if("BUNDLE" == his[0]){BUNDLE = his[1];}
            else if("BUNDLE_ID" == his[0]){BUNDLE_ID = his[1];}
            else if("FD_ID" == his[0]){FD_ID = his[1];}
            else{}
        }

        emit update_progress(BUNDLE,BUNDLE_ID,FD_ID,file->md5, "UPLOAD_COMPLETE", PCT.toFloat(),0);
    }
}

void FileManager::disconnected()
{

}


