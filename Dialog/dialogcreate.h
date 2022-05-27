#ifndef DIALOGCREATE_H
#define DIALOGCREATE_H

#include <QDialog>
#include <QPushButton>
#include <QMouseEvent>
#include <QLineEdit>
#include <QLabel>
#include <Component/BaseWindow.h>
#include <QMap>
#include <Data/UrlMeta.h>

namespace Ui {
class DialogCreate;
}

class DialogCreate : public BaseWindow
{
    Q_OBJECT

public:
    explicit DialogCreate(QWidget *parent = nullptr);
    ~DialogCreate();

    QLabel* title;
    QLineEdit* edit;
    QString type;
    QString typeName;
    QPushButton* btn_submit;
    QMap<QString,QString> type_map;
    UrlMeta* meta;

    void setType(QString);
    void setMeta(UrlMeta*);
    void init();
signals:
    //void create_succ();
    void intent(QString intent_name,QString intent_category, QString val); //CREATE,FOLDER,名称
private:
    Ui::DialogCreate *ui;
};

#endif // DIALOGCREATE_H
