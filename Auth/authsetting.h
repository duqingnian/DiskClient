#ifndef AUTHSETTING_H
#define AUTHSETTING_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class AuthSetting;
}

class AuthSetting : public QDialog
{
    Q_OBJECT

public:
    explicit AuthSetting(QWidget *parent = nullptr);
    ~AuthSetting();
    QSettings* _register;
private slots:
    void on_btn_cancel_clicked();
    void on_btn_submit_clicked();

signals:
    void save_setting();

private:
    Ui::AuthSetting *ui;
};

#endif // AUTHSETTING_H
