﻿#ifndef FDMENU_H
#define FDMENU_H

#include <QDialog>

#include <Data/MenuItem.h>

namespace Ui {
class FdMenu;
}

class FdMenu : public QDialog
{
    Q_OBJECT

public:
    explicit FdMenu(QWidget *parent = nullptr);
    ~FdMenu();

    QVector<MenuItem*> menus;
    void render_data();
    void render_ui();
    void set_suffix(QString);
    void set_enable(QString name, bool);
private:
    QString SUFFIX = "";
signals:
    void menu_clicked(QString);
private:
    Ui::FdMenu *ui;
};

#endif // FDMENU_H
