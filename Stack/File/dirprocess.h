#ifndef DIRPROCESS_H
#define DIRPROCESS_H

#include <QDialog>
#include <QTcpSocket>
#include <QTimer>

namespace Ui {
class DirProcess;
}

class DirProcess : public QDialog
{
    Q_OBJECT

public:
    explicit DirProcess(QWidget *parent = nullptr);
    ~DirProcess();
    QString ConverSize(unsigned long long bytes);
    void init(int fd_id);
    QTimer timer;
    int seconds;
    QString dir_path;
    QMovie *movie_loading;
    QMovie *movie_complete;

    //准备上传中
    void changeto_prepareing();

    //正在上传
    void changeto_uploading(int);

public slots:
    void set_data(QString name,QString data);
private:
    Ui::DirProcess *ui;
    int dir_count = 0;
    int file_count = 0;
    unsigned long long size = 0;
signals:
    void do_something(QString);
};

#endif // DIRPROCESS_H
