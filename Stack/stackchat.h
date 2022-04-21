#ifndef STACKCHAT_H
#define STACKCHAT_H

#include "Common/basecontroller.h"

class StackChat : public BaseController
{
    Q_OBJECT
public:
    explicit StackChat(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *) override;
};

#endif // STACKCHAT_H
