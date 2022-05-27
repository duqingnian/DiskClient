#pragma execution_character_set("utf-8")
#include "uploaddirthread.h"
#include <QCryptographicHash>
#include <QDataStream>
#include <QDir>

UploadDirThread::UploadDirThread(QObject *parent) : QThread(parent)
{
    socket = nullptr;
}

void UploadDirThread::set_dir(QString _dir)
{
    this->dir = _dir;
}

void UploadDirThread::set_socket_descriptor(qintptr descriptor)
{
    _descriptor = descriptor;
}

void UploadDirThread::run()
{
    if(socket == nullptr)
    {
        socket = new QTcpSocket();
        socket->setSocketDescriptor(_descriptor);
    }
    if(socket->isOpen())
    {
        this->loop_dir(this->dir,true);
    }
}

void UploadDirThread::loop_dir(QString path, bool _sub)
{
    QDir dir(path);
    if(!dir.exists())
        return;
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);    //过滤后只留下文件夹、文件以及隐藏文件，不包括当前文件夹和上级文件夹
    dir.setSorting(QDir::DirsFirst);    //优先排列文件夹

    foreach(QFileInfo info,dir.entryInfoList())
    {
        QThread::usleep(1);
        if(info.isFile())
        {
            QString filePath = path + "/" + info.fileName();
            unsigned long long filesize = info.size();
            QString suffix = info.suffix();

            QFile m_file(filePath);
            if(m_file.open(QIODevice::ReadOnly))
            {
                QDataStream socketStream(socket);
                socketStream.setVersion(QDataStream::Qt_5_12);

                int len = 0;
                unsigned long long left_size = m_file.size();
                int i = 0;

                int buf_size = 1024*64 - 128;
                bool running = true;

                do{
                    i++;

                    float PCT = 0.0f;
                    if(0 == left_size)
                    {
                        PCT = 100.00f;
                    }
                    else
                    {
                        PCT = static_cast<float>(filesize - left_size) / static_cast<float>(filesize);
                        PCT = PCT * 100;
                    }

                    QString meta = QString("ADOFILEMD5:%1,SUFFIX:%2,LEFT_SIZE:%3,BUF_SIZE:%4,PCT:%5;")
                            .arg(md5(filePath+QString::number(filesize)))
                            .arg(suffix)
                            .arg(QString::number(left_size))
                            .arg(QString::number(buf_size))
                            .arg(QString::number(PCT));

                    QByteArray data;
                    data.prepend(meta.toUtf8());
                    data.resize(128);

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
                qDebug() << "dir=" << info.absoluteDir();
                loop_dir(info.filePath(),false);
            }
        }
        if(_sub)
        {
            qDebug() << "上传结束了...";
        }
    }
}


QString UploadDirThread::md5(QString str)
{
    QString MD5;
    QByteArray _str;
    _str = QCryptographicHash::hash(str.toLatin1(),QCryptographicHash::Md5);
    MD5.append(_str.toHex());
    return MD5;
}
