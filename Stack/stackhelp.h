#ifndef STACKHELP_H
#define STACKHELP_H

#include <QDialog>
#include <QResizeEvent>
#include <QSettings>
#include <QUrl>
#include <Chromium/browser.h>
#include <Chromium/browserwindow.h>
#include <Common/basecontroller.h>

namespace Ui {
class StackHelp;
}

class StackHelp : public BaseController
{
    Q_OBJECT

public:
    explicit StackHelp(QWidget *parent = nullptr);
    ~StackHelp();
    QString base_path = "";
    QUrl path(QString);
    void resizeEvent(QResizeEvent *) override;
private:
    Ui::StackHelp *ui;
    QSettings* regedit;
    Browser *browser;
    BrowserWindow *window;
};

#endif // STACKHELP_H
