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
#include "dirprocess.h"
#include "filerow.h"
#include "uploadpannel.h"
#include <Component/FlowLayout.h>
#include <DropDown/dropdownupload.h>
#include <Menu/fdmenu.h>
#include <Menu/menufileexplorercanvas.h>
#include <QNetworkAccessManager>
#include "Common/filemanager.h"
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMovie>
#include <QProcess>
#include <QThread>
#include <QTimer>
#include <QTimer>
#include <Lib/HttpClient.h>
#include <Thread/processdirthread.h>
#include <Component/Msg.h>
#include <Data/meta.h>
#include "Component/Msg.h"

struct ATTRIBUTE {
    QString key;
    QString name;
    QString value;
};

class FileExplorer : public BaseController
{
    Q_OBJECT
public:

    explicit FileExplorer(QWidget *parent = nullptr);
     ~FileExplorer();

    bool in_loading = false;

    QTcpSocket* cmd_socket;
    QTcpSocket* download_socket;

    QString DisplayMod = "LIST"; //FLOW  LIST

    QStringList typesRes;

    QString USR_CACHE_DIR;

    QVector<FD*> fds;
    FD* selected_fd = NULL; //当前选中的FD
    QStringList office;

    int MAX_UPLOAD_FILES = 100; //一次最多可以上传的数量

    //设置当前页面的文件获取类型 我的文件? 部门文件? 群组文件?
    void set_meta(UrlMeta*);
    void change_folder(UrlMeta*);

    //ajax获取文件
    void load_files();
    //resize
    void flush(int,int);
    //渲染文件
    void render_files();
    void render_flow();
    void render_list();
    void render_list_header();
    void render_list_side();

    //整体画布
    QWidget* canvas;

    //遮罩
    QWidget* mask;

    //载入中
    QLabel* load;
    QLabel* loading_tip;
    void show_loading(QString tip="载入中");
    void hide_loading();
    void change_loading_tip(QString);

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

    //FLOW方式
    FlowLayout* flowLayout;
    QScrollArea* scrollArea;

    //list方式
    QWidget* file_row_header;
    QListWidget* listWapper;
    QWidget* ListSide;

    //上传文件夹 处理弹窗
    DirProcess* dir_process;

    void clear_flow_layout(); //清除flow布局子节点
    void cleat_list_rows();

    void fd_clicked(QMouseEvent* event,int id);

    //取消全部文件的选中状态
    void cancel_fd_selected();

    //打开文件
    void fd_open();

    //清空界面和数据
    void clear();

    //处理上传
    void PrepareIntentType(QString);

    //计算FlowLayout下的高度
    void calculate_flow_layout_height(bool hs=true);

    //将绝对路径的文件添加到队列里
    void moveto_queue(QString abs_file);

    //socket发消息
    bool sendMsgPack(QString _header, QString msg,QTcpSocket* socket);

    //渲染侧边栏文件信息
    void render_active_file_info();

    //文件后缀转图标
    QString suffix2icon(QString suffix);

    //没有任何文件的提示
    QLabel* EmptyTip;

    //上传文件夹
    QString upload_dir;

    UrlMeta* meta;
    QPoint pos;
    QFile *handle;

    UploadPannel* upload_pannel;

    //侧边栏
    QLabel* active_icon;  //图标
    QLabel* active_title; //标题
    QWidget* file_min_action; //吸底迷你动作

    QWidget* base_info; //基本信息
    QWidget* version_list; //版本列表

    int emp_width = 0;
    QListWidget* emplyees;
    Label* reload_album; //清除员工数据，重新载入部门数据
    QString SJN = ""; //selected job_number

    QTabWidget* side_tab;

    void render_base_info();
    void render_version_list();

    QString ConverSize(unsigned long long bytes);
    void render_attribute();

    //定位文件
    void localtion_file();

    //第一次运行的时候
    void OnStart();

    //渲染部门员工
    void render_employees(QString);

    //是否含有权限
    bool has_permission(QString permission_name);



    //文件夹上传相关
    //bool uploading = false;
    //void touch_upload(QString,int,int,QString);
    //void sync_file_progrrss(QString BUNDLE,QString BUNDLE_ID,QString FD_ID,QString md5,QString state,float pct,unsigned long long SPEED);
private:
    int _width;
    int _height;
    bool db_click = false;
    FD* active_fd;
    QString SHOW_EMP = "HIDE";
    unsigned long long total_size = 0;
    QMap<QString,int> DirIdMap;

    QStringList file_queue;

    //文件列表 表头
    Label* row_header_name;
    Label* row_header_user;
    Label* row_header_version;
    Label* row_header_size;

    //权限
    QMap<QString, int> permission;
    QMap<QString, EMP*> empMap;

    //上传的文件夹
    QMap<QString,int>* dirMap; //服务器创建后返回的路径和ID的集合
    QJsonArray dirArr;         //选择文件夹后的文件夹集合

public slots:
    void readyRead();
    void disconnected();

    //文件有变动
    void FileChanged(const QString &);

    //下载服务器
    void downloadReadyRead();

    void OpenCreateDropDown();  //打开创建下拉
    void OpenUploadDropDown();  //打开上传下拉
    void menu_clicked(QString);
    void fd_menu_clicked(QString);

    void select_employee(QListWidgetItem *item);
    void row_clicked(QListWidgetItem *item);
    void row_db_clicked(QListWidgetItem *item);
    void row_item_section_changed();

    void Refresh(); //刷新的槽函数

    void append_file(QString T,QString abs_file);

    void list_file(QString); //列出文件

    void dir_map(QString,QString); //处理目录map
signals:
    void append_urlbar(FD*);

    //更新上传文件的进度
    void sync_file_progrrss(QString BUNDLE,QString BUNDLE_ID,QString FD_ID,QString md5,QString state,float pct);

    //用户可见的部门和群组
    void sync_views(QString);

    //改变URL地址到部门
    void url_back_to(UrlMeta*);

    //重新载入部门文件数据
    void reload_meta_data(UrlMeta*);

    //同步权限
    void sync_permission(QString,int);
};

#endif // FILEEXPLORER_H
