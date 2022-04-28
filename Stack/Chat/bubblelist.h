#ifndef BUBBLELIST_H
#define BUBBLELIST_H

#include "bubbleinfo.h"

#include <QListWidget>

class BubbleList : public QListWidget
{
    Q_OBJECT
public:
    BubbleList();

    void insertBubble(BubbleInfo*);
    void addBubbleItem(BubbleInfo*);

private:
    QList<BubbleInfo*> bubbles;
};

#endif // BUBBLELIST_H
