#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QObject>
#include <QDialog>
#include <Common/basecontroller.h>
#include "TitleBar.h"
#include <QSettings>

class BaseWindow : public BaseController
{
    Q_OBJECT
public:
    explicit BaseWindow(QWidget *parent = nullptr);
    ~BaseWindow();
    void paintEvent(QPaintEvent *event);
    void loadStyleSheet(const QString &sheetName);
protected:
    void initTitleBar();
protected:
    TitleBar* m_TitleBar;
    int W;
    int H;

signals:

private slots:
    void onButtonMinClicked();
    void onButtonCloseClicked();
    void onButtonMaxClicked();
    void onButtonRestoreClicked();
};

#endif // BASEWINDOW_H
