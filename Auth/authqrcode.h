#ifndef AUTHQRCODE_H
#define AUTHQRCODE_H

#include <QDialog>
#include "Component/Label.h"
#include <QSettings>
#include "Common/basecontroller.h"

namespace Ui {
class AuthQrcode;
}

class AuthQrcode : public BaseController
{
    Q_OBJECT

public:
    explicit AuthQrcode(QWidget *parent = nullptr);
    ~AuthQrcode();

    QSettings* _register;

    QString qrcode_type = "dd";
    QString dd_auth_api;
    QString wechat_auth_api;

    void init();
    void updateQrCode(QLabel*,QString,int width=135, int height=135, int x=0,int y=0);
    void loop_result();
    void pause();

    int loop_count = 0;
    Label* tip;
    Label* qrcode_image;
    Label* auto_txt;
    Label* auto_check;
    QWidget* qrcode_border;
    bool remember = true;
    QWidget* invlid_mask;
    QString random_string = "";
    bool running;
public slots:
    void set_remember();
    void refresh_page();
    void tip_qrcode_invalid(QString tip="");
signals:
    void open_welcome(QString);

private:
    Ui::AuthQrcode *ui;
};

#endif // AUTHQRCODE_H
