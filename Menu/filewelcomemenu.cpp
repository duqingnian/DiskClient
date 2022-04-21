#include "filewelcomemenu.h"
#include "ui_filewelcomemenu.h"

FileWelcomeMenu::FileWelcomeMenu(QWidget *parent) : QDialog(parent), ui(new Ui::FileWelcomeMenu)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowStaysOnTopHint);
}

FileWelcomeMenu::~FileWelcomeMenu()
{
    delete ui;
}
