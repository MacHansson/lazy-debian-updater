#include "framelessdialog.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

FramelessDialog::FramelessDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_StaticContents);
    installEventFilter(this);
}

void FramelessDialog::showBeautiful()
{
    show();

    // did stutter outside of Qt - worked fine started from Qt
    /*
    setWindowOpacity(0.0); // Start invisible

    QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity");
    QObject::connect(fadeIn, &QPropertyAnimation::finished, this, [this] () {

        //

    });

    fadeIn->setDuration(400); // 250ms is usually the "sweet spot" for UI feel
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::Linear);

    show();
    raise();
    activateWindow();

    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    */
}

void FramelessDialog::hideBeautiful()
{
    if(m_pinned) {
        return;
    }

    hide();

    // did stutter outside of Qt - worked fine started from Qt
    /*
    QPropertyAnimation *fadeOut = new QPropertyAnimation(this, "windowOpacity");
    fadeOut->setDuration(250);
    fadeOut->setStartValue(windowOpacity()); // Start from current (usually 1.0)
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::Linear);

    QObject::connect(fadeOut, &QPropertyAnimation::finished, this, [this]() {
        hide();
        setWindowOpacity(1.0);
    });

    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    */
}

void FramelessDialog::setPinned(bool pin)
{
    m_pinned = pin;
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
            if(!m_pinned) {
                hideBeautiful();
            }
            return true;
        }
        default: return QDialog::eventFilter(watched, event);
    }
}
