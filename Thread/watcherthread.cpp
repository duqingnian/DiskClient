#include "watcherthread.h"
#include <QDebug>
#include <QDir>

WatcherThread::WatcherThread(QString usr_dir,QObject *parent) : QThread(parent)
{
    this->watch_path = usr_dir;
}

void WatcherThread::run()
{
    _resultEvent = CreateEvent(nullptr, true, false, nullptr);
    _stopEvent = CreateEvent(nullptr, true, false, nullptr);

    size_t bufferSize = 4096 * 10;
    const size_t maxBuffer = 64 * 1024;

    while (!_done) {
        bool increaseBufferSize = false;
        watchChanges(bufferSize, &increaseBufferSize);

        if (increaseBufferSize) {
            bufferSize = qMin(bufferSize * 2, maxBuffer);
        } else if (!_done) {
            sleep(2);
        }
    }
}

void WatcherThread::watchChanges(size_t fileNotifyBufferSize, bool *increaseBufferSize)
{
    *increaseBufferSize = false;
    _directory = CreateFileW(this->watch_path.toStdWString().data(),FILE_LIST_DIRECTORY,FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,nullptr);

    if (_directory == INVALID_HANDLE_VALUE) {
        const auto error = GetLastError();
        QString err = "Failed to create handle for" + _path + ", error:" + error;
        qDebug() << "err=" << err;
        _directory = 0;
        return;
    }

    OVERLAPPED overlapped;
    overlapped.hEvent = _resultEvent;

    QVarLengthArray<char, 4096 * 10> fileNotifyBuffer;
    fileNotifyBuffer.resize(static_cast<int>(fileNotifyBufferSize));

    const size_t fileNameBufferSize = 4096;
    TCHAR fileNameBuffer[fileNameBufferSize];

    pFileNotifyBuffer = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(fileNotifyBuffer.data());

    while (!_done) {
        ResetEvent(_resultEvent);
        DWORD dwBytesReturned = 0;
        if (!ReadDirectoryChangesW(_directory,
                                   pFileNotifyBuffer,
                                   static_cast<DWORD>(fileNotifyBufferSize),
                                   true,
                                   FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES,
                                   &dwBytesReturned,
                                   &overlapped,
                                   nullptr))
        {
            const DWORD errorCode = GetLastError();
            if (errorCode == ERROR_NOTIFY_ENUM_DIR) {
                notifi_change(_path,pFileNotifyBuffer);
                *increaseBufferSize = true;
            } else {
                qDebug() << "ReadDirectoryChangesW error:" << errorCode;
            }
            break;
        }

        emit ready();

        HANDLE handles[] = { _resultEvent, _stopEvent };
        DWORD result = WaitForMultipleObjects(2, handles,false,INFINITE);
        const auto error = GetLastError();
        if (result == 1) {
            qDebug() << "Received stop event, aborting folder watcher thread";
            break;
        }
        if (result != 0) {
            qDebug() << "WaitForMultipleObjects failed" << result << error;
            break;
        }

        bool ok = GetOverlappedResult(_directory, &overlapped, &dwBytesReturned, false);
        if (!ok) {
            const DWORD errorCode = GetLastError();
            if (errorCode == ERROR_NOTIFY_ENUM_DIR) {
                qDebug() << "The buffer for changes overflowed! Triggering a generic change and resizing";
                emit lostChanges();
                notifi_change(_path,pFileNotifyBuffer);
                *increaseBufferSize = true;
            } else {
                qDebug() << "GetOverlappedResult error" << errorCode;
            }
            break;
        }

        FILE_NOTIFY_INFORMATION *curEntry = pFileNotifyBuffer;
        forever {
            const int len = curEntry->FileNameLength / sizeof(wchar_t);
            QString longfile = this->watch_path + QString::fromWCharArray(curEntry->FileName, len);
            notifi_change(longfile,pFileNotifyBuffer);
            if (curEntry->NextEntryOffset == 0)
            {
                break;
            }
            curEntry = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(reinterpret_cast<char*>(curEntry) + curEntry->NextEntryOffset);
        }
    }

    CancelIo(_directory);
    closeHandle();
}

void WatcherThread::closeHandle()
{

}

void WatcherThread::notifi_change(const QString &path, FILE_NOTIFY_INFORMATION * curEntry)
{
    QFileInfo file_info(path);
    if (file_info.exists() && file_info.isFile())
    {
        //临时文件 略过
        QString file_name = file_info.fileName();
        if("~" == file_name.mid(0,1) || "." == file_name.mid(0,1))
        {
            return;
        }
        //临时文件和TMP后缀 略过
        QString suffix = "";
        if(file_name.lastIndexOf('.') > 0)
        {
            suffix = file_name.mid(file_name.lastIndexOf('.')+1);
        }

        if("" == suffix || "TMP" == suffix.toUpper())
        {
            return;
        }
        QStringList office_suffix = {"doc","docx","xls","xlsx","csv","ppt","pptx"};
        switch(curEntry->Action)
        {
        case FILE_ACTION_ADDED:
            qDebug() << "文件添加[" << path <<"]";
            break;
        case FILE_ACTION_REMOVED:
            qDebug() << "文件被删除[" << path <<"]";
            break;
        case FILE_ACTION_MODIFIED:
            qDebug() << "文件被修改[" << path <<"]";
            QThread::usleep(2);
            emit changed(path);
            break;
        case FILE_ACTION_RENAMED_OLD_NAME:
            if(office_suffix.contains(suffix))
            {
                qDebug() << "旧文件名[" << path <<"]";
                QThread::usleep(2);
                emit changed(path);
            }
            break;
        case FILE_ACTION_RENAMED_NEW_NAME:
            qDebug() << "新文件名[" << path <<"]";
            break;
        default:
            qDebug() << "未知操作=" << curEntry->Action << "[" << path <<"]";
            break;
        }
    }
}

WatcherThread::~WatcherThread()
{
    closeHandle();
}

void WatcherThread::stop()
{
    _done = 1;
    SetEvent(_stopEvent);
}


