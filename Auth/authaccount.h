#ifndef AUTHACCOUNT_H
#define AUTHACCOUNT_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <Common/basecontroller.h>
#include "Component/Label.h"

namespace Ui {
class AuthAccount;
}

class AuthAccount : public BaseController
{
    Q_OBJECT

public:
    explicit AuthAccount(QWidget *parent = nullptr);
    ~AuthAccount();

    QLineEdit* edit_account;
    QLineEdit* edit_password;
    QPushButton * btn_submit;

    Label* auto_txt;
    Label* auto_check;

    bool remember = true;

    void init();
    void do_login();

private slots:
    void set_remember();

private:
    Ui::AuthAccount *ui;

signals:
    void open_welcome(QString);

};

#endif // AUTHACCOUNT_H
