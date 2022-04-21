#ifndef FILEWELCOME_H
#define FILEWELCOME_H

#include <QWidget>
#include <Component/FlowLayout.h>
#include <QVector>
#include <QEvent>
#include <Data/Simple.h>
#include <Component/Label.h>
#include "Menu/filewelcomemenu.h"

class FileWelcome : public QWidget
{
    Q_OBJECT
public:
    explicit FileWelcome(QWidget *parent = nullptr);

    int width=1024;

    //我的文件
    void draw_myfile();
    //部门文件
    void draw_depfile();
    //群组文件
    void draw_groupfile();

    //渲染列表
    void render_deps();
    void render_groups();

    void clear_fd(FlowLayout*);

    QWidget* myfile_wapper;
    Label* myfile;
    Label* share_out;
    Label* share_in;

    //部门文件
    QVector<SIMPLE*> deps;
    QWidget* depfile_wapper;
    FlowLayout* dep_layout;

    //群组文件
    QLabel* label_group;
    QVector<SIMPLE*> groups;
    QWidget* groupfile_wapper;
    FlowLayout* group_layout;

    //右键菜单
    FileWelcomeMenu* menu;

    bool eventFilter(QObject*,QEvent*) override;
    void flush(int,int);
signals:
    void explorer(QString category,QString key,QString text,int id);
};

#endif // FILEWELCOME_H
