#include "welcome.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include "auth.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),QObject::tr("I couldn't detect any system tray on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    Auth _auth;
    if(_auth.exec() == QDialog::Accepted)
    {
        Welcome _welcome;
        _welcome.set_auth(&_auth);
        _welcome.show();
        return a.exec();
    }
    else
    {
        return 0;
    }
}
