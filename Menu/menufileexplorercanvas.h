#ifndef MENUFILEEXPLORERCANVAS_H
#define MENUFILEEXPLORERCANVAS_H

#include <QDialog>

#include <Data/MenuItem.h>

namespace Ui {
class MenuFileExplorerCanvas;
}

class MenuFileExplorerCanvas : public QDialog
{
    Q_OBJECT

public:
    explicit MenuFileExplorerCanvas(QWidget *parent = nullptr);
    ~MenuFileExplorerCanvas();

    QVector<MenuItem*> menus;
    void render_data();
    void render_ui();
signals:
    void menu_clicked(QString);
private:
    Ui::MenuFileExplorerCanvas *ui;
};

#endif // MENUFILEEXPLORERCANVAS_H
