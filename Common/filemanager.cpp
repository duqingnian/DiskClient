#pragma execution_character_set("utf-8")
#include "filemanager.h"
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QThread>

//构造函数
FileManager::FileManager(QObject *parent) : QThread(parent)
{
    socket = nullptr;
}

//设置文件
void FileManager::set_file(UP_FILE* _file)
{
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
        QString filePath = file->path;qDebug() << "filePath=" << filePath;

        QFile m_file(filePath);
        if(m_file.open(QIODevice::ReadOnly))
        {
            socket->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption,1024*32);
            QDataStream socketStream(socket);
            socketStream.setVersion(QDataStream::Qt_5_12);

            int len = 0;
            unsigned long long file_size = m_file.size();
            int send_size = 0;
            int i = 0;

            int buf_size = 1024*1024*2;

            do{
                i++;

                QString meta = QString("ADOFILEMD5:%1,SUFFIX:%2,LEFT_SIZE:%3,BUF_SIZE:%4,I:%5;")
                        .arg(file->md5)
                        .arg(file->suffix)
                        .arg(QString::number(file_size-send_size))
                        .arg(QString::number(buf_size))
                        .arg(QString::number(i));

                QByteArray header;
                header.prepend(meta.toUtf8());
                header.resize(128);

                QByteArray buf_str;
                buf_str = m_file.read(buf_size);

                len = buf_str.length();
                header.append(buf_str);

                socketStream << header;
                socket->flush();

                send_size += len;
                bool send_ret = socket->waitForBytesWritten(50000);
                qDebug() << meta << ",           send_ret=" << send_ret;
            }while((file_size-send_size) > 0);
        }
        else
        {
            qDebug() << "file opend failed!";
        }
    }

}

