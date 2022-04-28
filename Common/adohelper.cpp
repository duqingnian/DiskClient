#include "adohelper.h"

#include <QBitmap>
#include <QPainter>

AdoHelper::AdoHelper(QObject *parent) : QObject(parent)
{

}

QPixmap AdoHelper::PixmapToRound(const QPixmap &src, int radius)
{
    if (src.isNull()) {
        return QPixmap();
    }

    QSize size(2 * radius, 2 * radius);
    QBitmap mask(size);
    QPainter painter(&mask);

    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(0, 0, size.width(), size.height(), 100, 100);

    QPixmap image = src.scaled(size);
    image.setMask(mask);
    return image;
}

