#include "employeerow.h"
#include "ui_employeerow.h"

EmployeeRow::EmployeeRow(QWidget *parent) :QDialog(parent),ui(new Ui::EmployeeRow)
{
    ui->setupUi(this);
    this->setCursor(Qt::PointingHandCursor);
}

EmployeeRow::~EmployeeRow()
{
    delete ui;
}

void EmployeeRow::set_emp(EMP * _emp)
{
    ui->name->setText(_emp->name);
}
