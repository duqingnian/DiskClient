#pragma execution_character_set("utf-8")
#include "authsetting.h"
#include "ui_authsetting.h"
#include <QMessageBox>
#include <QSslSocket>

AuthSetting::AuthSetting(QWidget *parent) :QDialog(parent),ui(new Ui::AuthSetting)
{
    ui->setupUi(this);

    QString machineUniqueId = QString(QSysInfo::machineUniqueId()).toUpper();
    ui->label_machineUniqueId->setText(machineUniqueId);

    _register = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\AdoDisk", QSettings::NativeFormat);
    QString api_url = _register->value("api_url").toString();
    QString socket_server = _register->value("socket_server").toString();
    QString socket_port = _register->value("socket_port").toString();
    QString file_server = _register->value("file_server").toString();
    QString file_port = _register->value("file_port").toString();

    ui->edit_api_url->setText(api_url);
    ui->edit_socket_server->setText(socket_server);
    ui->edit_socket_port->setText(socket_port);
    ui->edit_file_server->setText(file_server);
    ui->edit_file_port->setText(file_port);

    ui->edit_sslLibraryBuildVersion->setText(QSslSocket::sslLibraryBuildVersionString());
    ui->edit_sslLibraryVersion->setText(QSslSocket::sslLibraryVersionString());
    ui->edit_supportsSsl->setText(QSslSocket::supportsSsl() ? "支持√" : "不支持×");
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
    //机器码
    QString machineUniqueId = QString(QSysInfo::machineUniqueId());
    _register->setValue("machineUniqueId",machineUniqueId);

    //接口地址
    QString edit_api_url = ui->edit_api_url->text();
    _register->setValue("api_url",edit_api_url);

    //消息服务器
    QString socket_server = ui->edit_socket_server->text();
    QString socket_port = ui->edit_socket_port->text();
    _register->setValue("socket_server",socket_server);
    _register->setValue("socket_port",socket_port);

    //文件服务器
    QString file_server = ui->edit_file_server->text();
    QString file_port = ui->edit_file_port->text();
    _register->setValue("file_server",file_server);
    _register->setValue("file_port",file_port);

    int ret = QMessageBox::information(NULL,"提示","已保存",QMessageBox::Ok);
    if(ret == QMessageBox::Ok)
    {
        //成功保存后，关闭弹窗，刷新二维码
        emit save_setting();
        this->close();
    }
}






