#ifndef CHATFRIENDITEM_H
#define CHATFRIENDITEM_H

#include <QDialog>
#include <QLabel>

#include <Data/User.h>

#include <Common/basecontroller.h>

namespace Ui {
class ChatFriendItem;
}

class ChatFriendItem : public BaseController
{
    Q_OBJECT

public:
    explicit ChatFriendItem(QWidget *parent = nullptr);
    ~ChatFriendItem();
    void set_employee(USER*);
    USER* get_employee();
    void selected();
    void unselected();
    void set_last_msg(QString);
    void set_last_time(QString);
private:
    Ui::ChatFriendItem *ui;
    USER* employee;
};

#endif // CHATFRIENDITEM_H
