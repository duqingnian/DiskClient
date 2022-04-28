#include "bubblelist.h"
#include "bubbleview.h"
#include <QDebug>

BubbleList::BubbleList()
{

}

void BubbleList::insertBubble(BubbleInfo* bi)
{
    bubbles.append(bi);
    addBubbleItem(bi);
}

void BubbleList::addBubbleItem(BubbleInfo* info)
{
    BubbleView * view = new BubbleView(nullptr,info);
    view->resize(this->width(),0);

    QListWidgetItem* item = new QListWidgetItem("");

    QSize size = view->setRect();
    view->setFixedSize(size);
    item->setSizeHint(size);

    this->addItem(item);
    this->setItemWidget(item,view);
}
