#ifndef AUTHSMS_H
#define AUTHSMS_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "Component/Label.h"

namespace Ui {
class AuthSms;
}

class AuthSms : public QDialog
{
    Q_OBJECT

public:
    explicit AuthSms(QWidget *parent = nullptr);
    ~AuthSms();

    QLineEdit* edit_account;
    QLineEdit* edit_password;
    QPushButton * btn_submit;
    QPushButton * btn_getcode;

    Label* auto_txt;
    Label* auto_check;

    bool remember = true;

    void init();


private slots:
    void set_remember();

private:
    Ui::AuthSms *ui;
};

#endif // AUTHSMS_H
