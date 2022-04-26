#ifndef STACKEMPTY_H
#define STACKEMPTY_H

#include <QWidget>
#include <QResizeEvent>
#include <QLabel>
#include <Common/basecontroller.h>

class StackEmpty : public BaseController
{
    Q_OBJECT
public:
    explicit StackEmpty(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *) override;
private:
    QLabel* start_tip;
};

#endif // STACKEMPTY_H
