#ifndef UPLOADPANNEL_H
#define UPLOADPANNEL_H

#include <QDialog>
#include <QListWidget>
#include <QScrollArea>
#include <QMap>
#include <QLabel>

#include <Data/UploadFileUnit.h>

namespace Ui {
class UploadPannel;
}

class UploadPannel : public QDialog
{
    Q_OBJECT

public:
    explicit UploadPannel(QWidget *parent = nullptr);
    ~UploadPannel();
    void add_upload(UP_FILE*);
private:
    Ui::UploadPannel *ui;
    QLabel* title;
    QWidget* topbar;
    QListWidget* upload_file_list;
    QMap<QString, UP_FILE*> uploads;
};

#endif // UPLOADPANNEL_H
