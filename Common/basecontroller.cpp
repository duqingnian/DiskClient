#include "basecontroller.h"
#include <QCoreApplication>

USER*    user      = new USER();
TABMETA* tab_meta  = new TABMETA();
FD*      fd        = new FD();

BaseController::BaseController(QWidget *parent) : QDialog(parent)
{
    regedit = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\AdoDisk", QSettings::NativeFormat);
    QString css  = "";
    css += "QScrollBar:vertical{border: 0px;background:#fff;width:5px;margin: 0px 0px 0px 0px;}";
    css += "QScrollBar::handle:vertical {min-height: 0px;border: 0px;border-radius: 0px;background-color: #CAC7C6;}";
    css += "QScrollBar::add-line:vertical {height: 0px;subcontrol-position: bottom;subcontrol-origin: margin;}";
    this->setStyleSheet(css);

    basepath = QCoreApplication::applicationDirPath();
}

BaseController::~BaseController()
{
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
    QDateTime time = QDateTime::currentDateTime();
    int timeT = time.toTime_t();
    return timeT;
}

QString BaseController::get_mtime()
{
    QDateTime time = QDateTime::currentDateTime();
    int timeT = time.toTime_t();

    int ms = time.time().msec();
    QString tmp = QString::number(timeT) + QString::number(ms);
    return tmp;

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
    QJsonDocument doc(jsonObj);
    QString str(doc.toJson(QJsonDocument::Compact));
    return str;
}

bool BaseController::sendmsg(QString sendto,QString msg,QString type)
{
    if("" == sendto)
        return false;
    if("" == type)
    {
        type = "TEXT";
    }
    QString header = "MSGTYPE:"+type+"|FROM:"+user->job_number+"|TO:"+sendto;
    return Socket::Instance()->send(header,msg);
}

bool BaseController::sendJsonObject(QString sendto,QJsonObject obj,QString type)
{
    return sendmsg(sendto,GetStrByQJsonObject(obj),type);
}


QString BaseController::md5_file(const QString &sourceFilePath)
{

    QFile sourceFile(sourceFilePath);
    qint64 fileSize = sourceFile.size();
    const qint64 bufferSize = 10240;

    if (sourceFile.open(QIODevice::ReadOnly)) {
        char buffer[bufferSize];
        int bytesRead;
        int readSize = qMin(fileSize, bufferSize);

        QCryptographicHash hash(QCryptographicHash::Md5);

        while (readSize > 0 && (bytesRead = sourceFile.read(buffer, readSize)) > 0) {
            fileSize -= bytesRead;
            hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        }

        sourceFile.close();
        return QString(hash.result().toHex());
    }
    return QString();
}
QPixmap BaseController::GetAvatar()
{
    return GetAvatar(user->job_number);
}
QPixmap BaseController::GetAvatar(QString job_number)
{
    if("SYS" == job_number)
    {
        return QPixmap(":/Resources/ico.ico");
    }
    QString ava = basepath+"/avatar/"+job_number.toUpper()+".jpg";
    QFile file(ava);
    if(file.exists())
    {
        return QPixmap::fromImage(QImage(ava));
    }
    return QPixmap::fromImage(QImage(":/Resources/User/noavatar_small.gif"));
}

void BaseController::wait( int ms )
{
    QElapsedTimer timer;
    timer.start();

    while ( timer.elapsed() < ms )
        QCoreApplication::processEvents();
}
