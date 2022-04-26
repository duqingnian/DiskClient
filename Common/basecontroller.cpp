#include "basecontroller.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QNetworkProxy>

USER*    user      = new USER();
TABMETA* tab_meta  = new TABMETA();
FD*      fd        = new FD();

BaseController::BaseController(QWidget *parent) : QDialog(parent)
{
    regedit = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\AdoDisk", QSettings::NativeFormat);

    QNetworkProxyFactory::setUseSystemConfiguration(false);
    socket = new QTcpSocket(this);
    server_ip = get_reg("socket_server");
    server_port = get_reg("socket_port");
}

BaseController::~BaseController()
{
    if(socket->isOpen())
    {
        socket->close();
        socket->deleteLater();
        socket = nullptr;
    }
}

//md5加密
QString BaseController::md5(QString str)
{
    QString MD5;
    QByteArray _str;
    _str = QCryptographicHash::hash(str.toLatin1(),QCryptographicHash::Md5);
    MD5.append(_str.toHex());
    return MD5;
}

//随机字符串
QString BaseController::random(int length)
{
    int time = get_time();
    QString tmp = md5(QString::number(time));
    QString str = tmp + QString::number(time+1);
    return str.mid(0,length);
}

//获取时间戳
int BaseController::get_time()
{
    QDateTime time = QDateTime::currentDateTime();   //获取当前时间
    int timeT = time.toTime_t();
    return timeT;
}

//根据注册表的键获取值
QString BaseController::get_reg(QString key)
{
    return regedit->value(key).toString();
}

//生成api接口地址
QString BaseController::path(QString route)
{
    return get_reg("api_url")+route;
}

SIMPLE BaseController::SimpleParse(QString res)
{
    SIMPLE simple;
    simple.id = "-1";
    simple.title = "请求解析错误，请稍后重试！";

    QJsonParseError err_rpt;
    QJsonDocument  jsonDoc = QJsonDocument::fromJson(res.toLatin1(), &err_rpt);
    if(err_rpt.error == QJsonParseError::NoError)
    {
        QJsonObject rootObj = jsonDoc.object();
        if(rootObj.contains("code"))
        {
            QJsonValue _code = rootObj.value("code");
            int code = _code.toInt();
            simple.id = QString::number(code);
            simple.title = rootObj.value("msg").toString();
        }else{
            simple.title = "Response don't contains code!";
        }
    }else{
        simple.title = "QJsonParseError";
    }

    return simple;
}

bool BaseController::is_cn(QChar)
{
    /*
    基本汉字	    20902字	4E00-9FA5
    基本汉字补充	90字	9FA6-9FFF
    扩展A	6592字	3400-4DBF
    扩展B	42720字	20000-2A6DF
    扩展C	4153字	2A700-2B738
    扩展D	222字	2B740-2B81D
    扩展E	5762字	2B820-2CEA1
    扩展F	7473字	2CEB0-2EBE0
    扩展G	4939字	30000-3134A
    */
    return true;
}

void BaseController::box(QString msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.exec();
}

QString BaseController::GetStrByQJsonObject(QJsonObject jsonObj)
{
    //QString strFromObj = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact).toStdString().c_str();
    /*
        QJsonDocument doc(jsonObj);
        QByteArray docByteArray = doc.toJson(QJsonDocument::Compact);
        Qstring strJson = QLatin1String(docByteArray);
    */
    QJsonDocument doc(jsonObj);
    QString str(doc.toJson(QJsonDocument::Compact));
    return str;
}

/*
 * header协议
 * 1.MSGTYPE:  协议类型
 * 2.FROM      来自，用户的job_number
 * 3.TO        发给谁，也是job_number
 * 4.STRUCT    用哪个结构体，方便服务器解包
 * 5.DATE_TYPE 数据类型，是json还是结构体，还是字符串
 *
 * data协议
 * header中的struct字符串
 */

bool BaseController::send(QString _header, QString _data)
{
    if(socket->isOpen())
    {
        QDataStream socketStream(socket);
        socketStream.setVersion(QDataStream::Qt_5_12);

        QByteArray header;
        header.prepend(_header.toUtf8());
        header.resize(128);

        QByteArray byteArray = _data.toUtf8();
        byteArray.prepend(header);

        socketStream << byteArray;
        byteArray.clear();

        return true;
    }
    else
    {
        qDebug() << "socket is closed!";
    }
    return false;
}

bool BaseController::sendJsonObject(QString header, QJsonObject obj)
{
    return send(header,GetStrByQJsonObject(obj));
}




















