#ifndef STACKFILEMAIN_H
#define STACKFILEMAIN_H

#include <QDialog>
#include <Component/Label.h>
#include "Common/basecontroller.h"
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>

#include "File/filewelcome.h"
#include "File/fileexplorer.h"

#include "Data/UrlMeta.h"

class StackFileMain : public BaseController
{
    Q_OBJECT
public:
    explicit StackFileMain(QWidget *parent = nullptr);

    //初始化导航栏
    void InitNavigate();

    //初始化动作栏
    void InitAction();

    //初始化内容区域
    void InitContent();

    //resize
    void resizeEvent(QResizeEvent *) override;

    //渲染地址栏
    void InitUrlBar();
    QHBoxLayout* url_layout;
    QVector<UrlMeta*> UrlMetas;
    void render_urlbar();
private:
    //导航
    QWidget* navigate;
    Label* quick_left;
    Label* quick_right;
    Label* quick_top;
    Label* quick_refresh;
    Label* quick_setting;
    QLineEdit* edit_search;
    QWidget* urlbar;

    //动作
    QWidget* ActionBar;
    QPushButton* btn_create;
    QPushButton* btn_upload;
    Label* ActSore;
    Label* ActLayout;
    Label* ActMore;

    //内容区域
    QStackedWidget* content;

    FileWelcome*   stack_file_welcome;
    FileExplorer*  stack_file_explorer;

private slots:
    void url_meta_clicked(UrlMeta*);
    void append_urlbar(FD*);
};

#endif // STACKFILEMAIN_H



