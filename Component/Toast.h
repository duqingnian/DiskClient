#ifndef TOAST_H
#define TOAST_H

#include <QDialog>

namespace Ui {
class Toast;
}

class Toast : public QDialog
{
    Q_OBJECT

public:
    explicit Toast(QWidget *parent = nullptr);
    ~Toast();
    void setText(const QString& text);
    void setBgColor(const QString& color);
    void showAnimation(int timeout = 3000);
public:
    static void tip(const QString& text);
    static void info(const QString& text);
    static void warn(const QString& text);
    static void err(const QString& text);
    static void succ(const QString& text);

    static void display(const QString& text,QString);
protected:
    virtual void paintEvent(QPaintEvent *event);
private:
    Ui::Toast *ui;
};

#endif // TOAST_H
