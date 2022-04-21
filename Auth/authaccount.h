#ifndef AUTHACCOUNT_H
#define AUTHACCOUNT_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "Component/Label.h"

namespace Ui {
class AuthAccount;
}

class AuthAccount : public QDialog
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


private slots:
    void set_remember();

private:
    Ui::AuthAccount *ui;
};

#endif // AUTHACCOUNT_H
