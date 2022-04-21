#pragma execution_character_set("utf-8")
#include "authsetting.h"
#include "ui_authsetting.h"
#include <QMessageBox>

AuthSetting::AuthSetting(QWidget *parent) :QDialog(parent),ui(new Ui::AuthSetting)
{
    ui->setupUi(this);

    QString machineUniqueId = QString(QSysInfo::machineUniqueId());
    ui->label_machineUniqueId->setText(machineUniqueId);

    _register = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\AdoDisk", QSettings::NativeFormat);
    QString api_url = _register->value("api_url").toString();
    QString socket_server = _register->value("socket_server").toString();
    QString socket_port = _register->value("socket_port").toString();

    ui->edit_api_url->setText(api_url);
    ui->edit_socket_server->setText(socket_server);
    ui->edit_socket_port->setText(socket_port);
}

AuthSetting::~AuthSetting()
{
    delete ui;
}

void AuthSetting::on_btn_cancel_clicked()
{
    this->hide();
}

//保存配置信息
void AuthSetting::on_btn_submit_clicked()
{
    QString edit_api_url      = ui->edit_api_url->text();
    QString socket_server = ui->edit_socket_server->text();
    QString socket_port = ui->edit_socket_port->text();

    QString machineUniqueId = QString(QSysInfo::machineUniqueId());

    _register->setValue("api_url",edit_api_url);
    _register->setValue("socket_server",socket_server);
    _register->setValue("machineUniqueId",machineUniqueId);
    _register->setValue("socket_port",socket_port);

    int ret = QMessageBox::information(NULL,"提示","已保存",QMessageBox::Ok);
    if(ret == QMessageBox::Ok)
    {
        //成功保存后，关闭弹窗，刷新二维码
        emit save_setting();
        this->close();
    }
}






