#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include "Common/basecontroller.h"
#include <QEvent>
#include <QMouseEvent>
#include <QWidget>
#include <QPoint>
#include <QScrollArea>
#include <QNetworkReply>
#include "Data/UrlMeta.h"
#include "DropDown/dropdowncreate.h"
#include "Dialog/dialogcreate.h"
#include "uploadpannel.h"
#include <Component/FlowLayout.h>
#include <DropDown/dropdownupload.h>
#include <Menu/fdmenu.h>
#include <Menu/menufileexplorercanvas.h>
#include <QNetworkAccessManager>

class FileExplorer : public BaseController
{
    Q_OBJECT
public:

    explicit FileExplorer(QWidget *parent = nullptr);

    QVector<FD*> fds;
    FD* selected_fd = NULL;

    //设置当前页面的文件获取类型 我的文件? 部门文件? 群组文件?
    void set_meta(UrlMeta*);

    void change_folder(UrlMeta*);

    //ajax获取文件
    void load_files();
    //resize
    void flush(int,int);
    //渲染文件
    void render_files();

    //整体画布
    QWidget* canvas;
    //文件渲染方式，列表还是块
    QString file_render_type = "list";//list block

    //画布右键菜单
    MenuFileExplorerCanvas* MenuCanvas;
    //文件右键菜单
    FdMenu* fd_menu;

    //创建下拉
    DropDownCreate* dropdown_create;
    //上传下拉
    DropDownUpload* dropdown_upload;

    //事件过滤
    bool eventFilter(QObject*,QEvent*) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    //新建弹窗
    DialogCreate* dlg_create;

    FlowLayout* flowLayout;
    QScrollArea* scroll;

    void clear_flow_layout();

    void fd_clicked(QMouseEvent* event,int id);

    //取消全部文件的选中状态
    void cancel_fd_selected();

    //打开文件
    void fd_open();

    //处理上传
    void PrepareIntentType(QString);

//    //上传队列
//    QVector<QString> upload_queue;

//    //显示上传面板
//    void show_upload_pannel();

    //没有任何文件的提示
    QLabel* EmptyTip;

    QNetworkAccessManager* AccessManage;
    QNetworkReply *reply;

    UrlMeta* meta;
    QPoint pos;
    QFile *handle;

    UploadPannel* upload_pannel;

public slots:
    void OpenCreateDropDown();  //打开创建下拉
    void OpenUploadDropDown();  //打开上传下拉
    void menu_clicked(QString);
    void fd_menu_clicked(QString);

    //上传文件
    void replyFinished(QNetworkReply*);
    void loadError(QNetworkReply::NetworkError);
    void loadProgress(qint64 ,qint64);
signals:
    void append_urlbar(FD*);

};

#endif // FILEEXPLORER_H
