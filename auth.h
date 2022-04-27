#ifndef AUTH_H
#define AUTH_H

#include <QDialog>
#include "Component/BaseWindow.h"
#include "Component/Label.h"
#include <QStackedWidget>
#include <Auth/authqrcode.h>
#include <Auth/authaccount.h>
#include <Auth/authsms.h>
#include "Auth/authsetting.h"

namespace Ui {
class Auth;
}

class Auth : public BaseWindow
{
    Q_OBJECT

public:
    explicit Auth(QWidget *parent = nullptr);
    ~Auth();
    void closeEvent(QCloseEvent *event);

    void init();
    void init_register();

    QString auth_type="qrcode";
    Label* auth_type_qrcode;
    Label* auth_type_account;
    Label* auth_type_sms;
    AuthSetting* dlg_setting;

    QSettings* _register;
    //注册表配置参数
    QString uid = "";
    QString api_url = "";
    QString socket_server = "";
    QString machineUniqueId = ""; //电脑唯一ID

    //尝试连接socket
    bool try_connect_server();

    //stack组件
    QStackedWidget* auth_canvas;

    //二维码登录组件
    AuthQrcode* auth_widget_qrcode;
    //账号密码登录组件
    AuthAccount* auth_widget_account;
    //短信登录组件
    AuthSms* auth_widget_sms;

    //渲染登录方式
    void render_auth_component();

    //自动登录
    void autologin(QString);

    //自动登录失败
    void init_stack_widgets();

private slots:
    void set_auth_type(QString);
    void open_setting_dialog();
    void welcome(QString);

signals:
    void login_success();

private:
    Ui::Auth *ui;
};

#endif // AUTH_H






