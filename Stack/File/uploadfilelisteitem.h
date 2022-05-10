#ifndef UPLOADFILELISTEITEM_H
#define UPLOADFILELISTEITEM_H

#include <QDialog>
#include <Data/UploadFileUnit.h>

namespace Ui {
class UploadFileListeItem;
}

class UploadFileListeItem : public QDialog
{
    Q_OBJECT

public:
    explicit UploadFileListeItem(QWidget *parent = nullptr);
    ~UploadFileListeItem();

    UP_FILE* upload_file;
    void set_upload_file(UP_FILE*);

    QString ConverSize(unsigned long long);

    void set_progress(float);

private:
    Ui::UploadFileListeItem *ui;
};

#endif // UPLOADFILELISTEITEM_H
