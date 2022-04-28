#ifndef ADOHELPER_H
#define ADOHELPER_H

#include <QObject>
#include <QPixmap>

class AdoHelper : public QObject
{
public:
    explicit AdoHelper(QObject *parent = nullptr);
    static QPixmap PixmapToRound(const QPixmap &src, int radius);
signals:

};

#endif // ADOHELPER_H
