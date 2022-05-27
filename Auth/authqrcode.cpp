#pragma execution_character_set("utf-8")
#include "authqrcode.h"
#include "ui_authqrcode.h"
#include "Lib/qrcodegen.hpp"
#include <QPushButton>
#include <QDebug>
#include <QJsonParseError>
#include <QMessageBox>
#include <QJsonObject>
#include <QTimer>
#include <Lib/HttpClient.h>

AuthQrcode::AuthQrcode(QWidget *parent) : BaseController(parent) , ui(new Ui::AuthQrcode)
{
    ui->setupUi(this);

    qrcode_border = new QWidget(this);
    qrcode_border->setObjectName("qrcode_border");
    qrcode_border->resize(135,135);
    qrcode_border->move(100,45);
    qrcode_border->setStyleSheet("#qrcode_border{border-width:10px;border-image: url(:/Resources/shadow_wnd_bg.png) 16 16 16 16;}");

    invlid_mask = new QWidget(this);
    invlid_mask->setObjectName("invlid_mask");
    invlid_mask->resize(125,125);
    invlid_mask->move(105,50);
    invlid_mask->resize(qrcode_border->width()-10,qrcode_border->height()-10);
    invlid_mask->setStyleSheet("#invlid_mask{background:url(:/Resources/opacity9.png);}");

    init();
}

AuthQrcode::~AuthQrcode()
{
    delete ui;
}


void AuthQrcode::updateQrCode(QLabel* _label,QString text, int width, int height, int x, int y)
{
    using namespace qrcodegen;
    QrCode qr = QrCode::encodeText( text.toUtf8().data(), QrCode::Ecc::MEDIUM );
    qint32 sz = qr.getSize();
    QImage im(sz,sz, QImage::Format_RGB32);
    QRgb black = qRgb(  0,  0,  0);
    QRgb white = qRgb(255,255,255);
    for (int y = 0; y < sz; y++)
        for (int x = 0; x < sz; x++)
            im.setPixel(x,y,qr.getModule(x, y) ? black : white );
    _label->setPixmap( QPixmap::fromImage(im.scaled(width,height,Qt::KeepAspectRatio,Qt::FastTransformation),Qt::MonoOnly) );
    _label->move(x,y);
}

//提示二维码失效了
void AuthQrcode::tip_qrcode_invalid(QString tip)
{
    QLabel* label_txt = new QLabel(invlid_mask);
    label_txt->setObjectName("label_txt");
    label_txt->resize(invlid_mask->width(),30);
    label_txt->move(0,30);
    label_txt->setAlignment(Qt::AlignCenter);
    if("" == tip)
    {
        label_txt->setText("二维码已失效");
    }
    else
    {
        label_txt->setText(tip);
    }
    label_txt->setStyleSheet("#label_txt{font-size:14px;color:#333;font-family: \"Microsoft Yahei\";}");

    QPushButton* refresh = new QPushButton(invlid_mask);
    refresh->setObjectName("refresh");
    refresh->setText("刷新二维码");
    refresh->resize(90,30);
    refresh->move(20,65);
    refresh->setCursor(Qt::PointingHandCursor);
    refresh->setStyleSheet("#refresh{background-color:#637DFF;color:#fff;border-radius:15px;font-family: \"Microsoft Yahei\";}");
    invlid_mask->show();
    qDebug() << "tip_qrcode_invalid:" << tip;
    connect(refresh,&QPushButton::clicked,this,[this]{refresh_page();});
}

void AuthQrcode::init()
{
    tip = new Label(this);
    tip->setText("请使用钉钉手机APP扫码登录");
    tip->setObjectName("tip");
    tip->move(0,200);
    tip->resize(348,30);
    tip->setAlignment(Qt::AlignCenter);
    tip->setStyleSheet("#tip{font-size:14px;color:#333;font-family: \"Microsoft Yahei\";}");

    //是否自动登录单选框
    auto_check = new Label(this);
    auto_check->setObjectName("auto_check");
    auto_check->move(115,235);
    auto_check->resize(20,20);
    auto_check->setStyleSheet("#auto_check{border-image:url(:/Resources/Form/listhead_check.png) 0 0 0 20;}");
    auto_check->setCursor(Qt::PointingHandCursor);
    connect(auto_check,&Label::clicked,this,&AuthQrcode::set_remember);

    auto_txt = new Label(this);
    auto_txt->setText("下次自动登录");
    auto_txt->setObjectName("auto_txt");
    auto_txt->move(136,237);
    auto_txt->setCursor(Qt::PointingHandCursor);
    auto_txt->setStyleSheet("#auto_txt{font-size:12px;color:#B0B1B3;font-family: \"Microsoft Yahei\";}");
    connect(auto_txt,&Label::clicked,this,&AuthQrcode::set_remember);

    //生成二维码
    _register = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\AdoDisk", QSettings::NativeFormat);

    qrcode_image = new Label(qrcode_border);
    qrcode_image->setObjectName("qrcode_image");
    qrcode_image->move(0,0);
    qrcode_image->setCursor(Qt::PointingHandCursor);
    qrcode_image->resize(qrcode_border->width(),qrcode_border->height());
    connect(qrcode_image,&Label::clicked,this,[this]{refresh_page();});

    refresh_page();
}

void AuthQrcode::set_remember()
{
    if(remember)
    {
        remember = false;
        auto_check->setStyleSheet("#auto_check{border-image:url(:/Resources/Form/listhead_check.png) 0 20 0 0;}");
    }
    else
    {
        remember = true;
        auto_check->setStyleSheet("#auto_check{border-image:url(:/Resources/Form/listhead_check.png) 0 0 0 20;}");
    }
}

//刷新二维码
void AuthQrcode::refresh_page()
{
    QString auth_api = "";
    if(qrcode_type == "dd")
    {
        auth_api = "http://disk.czmylike.com/client/auth.ding";
    }
    else if(qrcode_type == "wechat")
    {
        auth_api = "http://disk.czmylike.com/client/auth.wechat";
    }
    if(auth_api.length() < 30 || !auth_api.contains("http"))
    {
        updateQrCode(qrcode_image,"暂无配置信息，无法生成二维码，请先配置必须参数！",115,120, 10, 0);
        tip_qrcode_invalid("无法生成二维码");
    }
    else
    {
        int t = get_time();
        random_string = random(16);
        loop_count = 0;
        QString time = QString::number(t);
        QString token = md5("QRCODE|DD|DUQINGNIAN|"+random_string+"|2022|"+time+"|MYLIKE");
        QString url = auth_api+"?type=qrcode&app=dd&rand="+random_string+"&time="+time+"&token="+token;
        updateQrCode(qrcode_image,url,115,120, 10, 0);
        invlid_mask->hide();

        //生成二维码后，开始轮询结果
        loop_result();
    }
}

void AuthQrcode::pause()
{
    running = false;
}

void AuthQrcode::loop_result()
{
    if(!running)
    {
        return ;
    }
    QString api_url =  "http://disk.czmylike.com/";
    HttpClient(api_url+"client/auth/ding/sync.html").success([this](const QString &response) {
        QJsonParseError err_rpt;
        QJsonDocument  jsonDoc = QJsonDocument::fromJson(response.toUtf8(), &err_rpt);
        if(err_rpt.error != QJsonParseError::NoError)
        {
            QMessageBox msgBox;
            msgBox.setText("登录时服务器返回了错误的数据格式");
            msgBox.exec();
        }
        else
        {
            QJsonObject rootObj = jsonDoc.object();
            if(!rootObj.contains("code"))
            {
                QMessageBox msgBox;
                msgBox.setText("服务器未返回code");
                msgBox.exec();
            }
            else
            {
                QJsonValue _code = rootObj.value("code");
                int code = _code.toInt();
                if(0 == code)
                {
                    emit open_welcome(response.toUtf8());
                }
                else
                {
                    QJsonValue _msg = rootObj.value("msg");
                    QString msg = _msg.toString();
                    if("SYNC_NEXT" == msg)
                    {
                        loop_count++;
                        if(loop_count < 20){
                            QTimer::singleShot(3000, this, &AuthQrcode::loop_result);
                        }else{
                            tip_qrcode_invalid();
                        }
                    }
                    else
                    {
                        QMessageBox msgBox;
                        msgBox.setText(msg);
                        msgBox.exec();
                    }
                }
            }
        }

    }).param("rand", random_string).header("rand", random_string).header("token", md5(random_string)).header("content-type", "application/x-www-form-urlencoded").post();
}










