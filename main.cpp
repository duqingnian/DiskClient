#include "welcome.h"
#include <QApplication>
#include "auth.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Auth _auth;
//    if(_auth.exec() == QDialog::Accepted)
//    {
//        Welcome _welcome;
//        _welcome.set_auth(&_auth);
//        _welcome.show();
//        return a.exec();
//    }
//    else
//    {
//        return 0;
//    }

    Welcome _welcome;
    //_welcome.set_auth(&_auth);
    _welcome.show();
    return a.exec();
}
