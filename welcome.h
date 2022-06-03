#ifndef WELCOME_H
#define WELCOME_H

#include <QDialog>
#include <QMap>
#include <QSystemTrayIcon>
#include "auth.h"
#include <QVector>
#include <QStackedWidget>
#include <Stack/stackchat.h>
#include <Stack/stackfileMain.h>
#include <Stack/stackhelp.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Welcome; }
QT_END_NAMESPACE

class Welcome : public BaseWindow
{
    Q_OBJECT

public:
    Welcome(QWidget *parent = nullptr);
    ~Welcome();
    QScreen *screen;
    int _width = 1024;
    int _height = 768;

    //托盘
    QAction *maxAction;
    QAction *showAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    void createActions();
    void createTrayIcon();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    /////////////////////////////////////////////

    QMap<QString,TABMETA*> left_tab_metas;
    QMap<QString,TABMETA*> right_tab_metas;

    QWidget* TAB;
    QWidget* TAB_ATTR;
    QString ActiveTAB=""; //当前选中的TAB
    QVector<Label*> TabItems;
    //auth对象
    Auth* auth;
    QStackedWidget* StackWidget;

    StackFileMain* stack_file;
    StackChat* stack_chat;
    StackHelp* stack_help;

    //设置auth对象
    void set_auth(Auth*);

    //初始化Layout
    void InitLayout();

    //渲染全局tab
    void render_tab();
    void render_tab_attr();

    void resizeEvent(QResizeEvent *) override;

private slots:
    //点击了tab
    void SelectTab(QString);

private:
    Ui::Welcome *ui;
};
#endif // WELCOME_H












