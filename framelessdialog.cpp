#include "framelessdialog.h"

FramelessDialog::FramelessDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    installEventFilter(this);
}

void FramelessDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartPos = event->globalPosition().toPoint();
        m_windowStartPos = this->frameGeometry().topLeft();
        event->accept();
    } else {
        QDialog::mousePressEvent(event);
    }
}

void FramelessDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint currentPos = event->globalPosition().toPoint();
        QPoint delta = currentPos - m_dragStartPos;
        this->move(m_windowStartPos + delta);
        event->accept();
    } else {
        QDialog::mouseMoveEvent(event);
    }
}

void FramelessDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_dragging && event->button() == Qt::LeftButton) {
        m_dragging = false;
        emit positionChanged(this->pos());
        event->accept();
    } else {
        QDialog::mouseReleaseEvent(event);
    }
}

bool FramelessDialog::eventFilter(QObject* watched, QEvent* event) {
    switch(event->type()) {
        case QEvent::WindowActivate: {
            // nothing to do here
            return true;
        }
        case QEvent::WindowDeactivate: {
            setVisible(false);
            return true;
        }
        default: return QDialog::eventFilter(watched, event);
    }
}
