#include "Component/Label.h"
#include <QMouseEvent>

Label::Label(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent) {

}

Label::~Label() {}

void Label::mousePressEvent(QMouseEvent* event) {
    emit mouse_press(event);
    if(event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
}

void Label::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit mouse_press(event);
    emit dbclicked();
}
