#include "stackhelp.h"
#include "ui_stackhelp.h"


StackHelp::StackHelp(QWidget *parent) :BaseController(parent),ui(new Ui::StackHelp)
{
    ui->setupUi(this);

}

StackHelp::~StackHelp()
{
    delete ui;
}

