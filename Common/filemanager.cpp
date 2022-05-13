#include "filemanager.h"
#include <QDataStream>
#include <QMutex>
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QElapsedTimer>
#include <QCoreApplication>

FileManager *FileManager::Instance()
{
    static QMutex mutex;
    static QScopedPointer<FileManager> inst;
    if (Q_UNLIKELY(!inst)) {
        mutex.lock();
        if (!inst) {
            inst.reset(new FileManager);
        }
        mutex.unlock();
    }
    return inst.data();
}

QString FileManager::getErr()
{
    return err;
}

bool FileManager::isOpen()
{
    return socket->isOpen();
}

bool FileManager::upload(QString md5, UP_FILE* file)
{
    if(socket)
    {
        if(socket->isOpen())
        {
            QString filePath = file->path;

            QFile m_file(filePath);
            if(m_file.open(QIODevice::ReadOnly))
            {
                QDataStream socketStream(socket);
                socketStream.setVersion(QDataStream::Qt_5_12);

                int len = 0;
                unsigned long long file_size = m_file.size();
                int send_size = 0;
                int i = 1;

                int buf_size = 1024*64;

                //小于200M
                if(file_size < 200 * 1024 * 1024)
                {
                    buf_size = 1024*1024*2;
                }
                else if(file_size < 500 * 1024 * 1024) //小于500M
                {
                    buf_size = 1024*1024*4;
                }
                else if(file_size < 1024 * 1024 * 1024) //小于1GB
                {
                    buf_size = 1024*1024*8;
                }
                else
                {
                    buf_size = 1024*1024*16;
                }

                do{
                    i++;
                    QString meta = QString("ADOFILEMD5:%1,SUFFIX:%2,LEFT_SIZE:%3,BUF_SIZE:%4,I:%5;")
                            .arg(file->md5)
                            .arg(file->type)
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

                    socketStream.setVersion(QDataStream::Qt_5_12);
                    socketStream << header;

                    send_size += len;

                    socket->waitForBytesWritten();
                }
                while((file_size-send_size) > 0);

                return true;
            }
            else
            {
                qDebug() << "file opend failed!";
            }
        }
        else
        {
            qDebug() << "QTCPsocket Socket doesn't seem to be opened";
        }
    }
    else
    {
        qDebug() << "QTCPsocket Not connected";
    }
    return false;
}

void FileManager::wait( int ms )
{
    QElapsedTimer timer;
    timer.start();

    while ( timer.elapsed() < ms )
        QCoreApplication::processEvents();
}

QTcpSocket* FileManager::handle()
{
    return socket;
}

void FileManager::disconnect()
{
    if(socket && socket->isOpen())
    {
        socket->disconnect();
    }
    socket->deleteLater();
}

FileManager::FileManager(QObject *parent) : QObject(parent)
{
    QSettings* regedit  = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\AdoDisk", QSettings::NativeFormat);
    QString file_server   = regedit->value("file_server").toString();
    QString file_port = regedit->value("file_port").toString();

    if("" != file_server && "" != file_port)
    {
        socket = new QTcpSocket(this);
        socket->abort();
        if(!socket->isOpen())
        {
            socket->connectToHost(file_server,file_port.toInt());
            if(!socket->waitForConnected())
            {
                err = QString("文件服务器连接失败, err:%1.").arg(socket->errorString());
                qDebug() << err;
            }
        }
    }
}
