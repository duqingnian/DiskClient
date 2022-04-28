#ifndef BUBBLEINFO_H
#define BUBBLEINFO_H

#include <QObject>
#include <Data/User.h>
#include "Data/Pdu.h"

class BubbleInfo : public QObject
{
    Q_OBJECT
public:
    explicit BubbleInfo(QObject *parent = nullptr);

    USER* msg_sendor;
    QString msg = "";
    MSGTYPE msgtype;
    qint64 time = 0;

signals:

};

#endif // BUBBLEINFO_H
