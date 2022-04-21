#ifndef FILEWELCOMEMENU_H
#define FILEWELCOMEMENU_H

#include <QDialog>

namespace Ui {
class FileWelcomeMenu;
}

class FileWelcomeMenu : public QDialog
{
    Q_OBJECT

public:
    explicit FileWelcomeMenu(QWidget *parent = nullptr);
    ~FileWelcomeMenu();

private:
    Ui::FileWelcomeMenu *ui;
};

#endif // FILEWELCOMEMENU_H
