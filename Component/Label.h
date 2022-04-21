#ifndef LABEL_H
#define LABEL_H

#include <QLabel>
#include <QWidget>
#include <Qt>

class Label : public QLabel {
    Q_OBJECT

public:
    explicit Label(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~Label();

signals:
    void mouse_press(QMouseEvent*);
    void clicked();
    void dbclicked();

public:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

};

#endif // LABEL_H
