#include "stackhelp.h"
#include "ui_stackhelp.h"

#include "Chromium/tabwidget.h"
#include <QNetworkProxy>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <Chromium/webview.h>

StackHelp::StackHelp(QWidget *parent) :BaseController(parent),ui(new Ui::StackHelp)
{
    ui->setupUi(this);
    this->setStyleSheet("font-family: \"Microsoft Yahei\";");
    //获取注册表里面的开始网址
    regedit = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\AdoDisk", QSettings::NativeFormat);
    base_path = regedit->value("start_page").toString();
    if("" == base_path)
    {
        base_path = "http://172.19.159.136/disk/web/";
    }
    else if("http" != base_path.mid(0,4))
    {
        base_path = "http://172.19.159.136/disk/web/";
    }
    else
    {}

    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    QWebEngineProfile::defaultProfile()->setUseForGlobalCertificateVerification();
#endif

    QNetworkProxyFactory::setUseSystemConfiguration(false);

    browser = new Browser(this);
    window = browser->createWindow();
    window->tabWidget()->setUrl(path("activity/start"));

    window->tabWidget()->move(100,120);
    window->tabWidget()->resize(800,520);

    //更新日志
    WebView *help_view = window->tabWidget()->createTab();
    help_view->setUrl(path("activity/update_log"));

    window->tabWidget()->setCurrentIndex(0);
}

StackHelp::~StackHelp()
{
    delete ui;
}

QUrl StackHelp::path(QString url)
{
    QString rand = random(4).toUpper();
    QString ts = QString::number(get_time());
    QString token = md5("DQN10985"+user->uid+user->job_number+ts+rand);
    QString _url = base_path+url+"?uid="+user->uid+"&job_number="+user->job_number+"&ts="+ts+"&rand="+rand+"&token="+token;
    qDebug() << "_url=" << _url;
    return QUrl(_url);
}

void StackHelp::resizeEvent(QResizeEvent*)
{
    window->resize(this->width(),this->height()+22);
}
