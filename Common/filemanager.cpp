#pragma execution_character_set("utf-8")
#include "filemanager.h"
#include <QDataStream>
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
void FileManager::set_file(QString meta_key,int meta_id,int fd_id,UP_FILE* _file)
{
    META_KEY = meta_key;
    META_ID = meta_id;
    FD_ID = fd_id;
    this->file = _file;
}

//设置socket的描述符
void FileManager::set_socket_descriptor(qintptr descriptor)
{
    _descriptor = descriptor;
}

void FileManager::run()
{
    if(socket == nullptr)
    {
        socket = new QTcpSocket();
        socket->setSocketDescriptor(_descriptor);
    }
    if(socket->isOpen())
    {
        QString filePath = file->path;

        QFile m_file(filePath);
        if(m_file.open(QIODevice::ReadOnly))
        {
            QDataStream socketStream(socket);
            socketStream.setVersion(QDataStream::Qt_5_15);

            int len = 0;
            unsigned long long left_size = m_file.size();

            int buf_size = 1024*64 - DATA_META_LEN;
            bool running = true;

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
                QString meta = QString("ADOFILE:?MD5:%1,SUFFIX:%2,LEFT_SIZE:%3,BUF_SIZE:%4,FS:%5,PCT:%6,BUNDLE:%7,BUNDLE_id:%8,FD_ID:%9;")
                        .arg(file->md5)
                        .arg(file->suffix)
                        .arg(QString::number(left_size))
                        .arg(QString::number(buf_size))
                        .arg(QString::number(m_file.size()))
                        .arg(QString::number(PCT))
                        .arg(META_KEY).arg(QString::number(META_ID)).arg(QString::number(FD_ID));

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

                left_size -= len;
                socket->waitForBytesWritten();

                if(!running)
                {
                    socketStream << "ENDL";
                    socket->flush();
                    socket->waitForBytesWritten();
                    data.clear();
                }

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
}


