#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <Common/basecontroller.h>
#include <QLabel>

class Chat : public BaseController
{
    Q_OBJECT
public:
    explicit Chat(QWidget *parent = nullptr);

signals:

};

#endif // CHAT_H
