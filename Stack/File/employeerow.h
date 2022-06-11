#ifndef EMPLOYEEROW_H
#define EMPLOYEEROW_H

#include <QDialog>

#include <Data/meta.h>

namespace Ui {
class EmployeeRow;
}

class EmployeeRow : public QDialog
{
    Q_OBJECT

public:
    explicit EmployeeRow(QWidget *parent = nullptr);
    ~EmployeeRow();

    void set_emp(EMP*);
private:
    Ui::EmployeeRow *ui;
};

#endif // EMPLOYEEROW_H
