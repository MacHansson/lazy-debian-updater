#ifndef FRAMELESSDIALOG_H
#define FRAMELESSDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QPoint>

class FramelessDialog : public QDialog
{
    Q_OBJECT

public:

    explicit FramelessDialog(QWidget *parent = nullptr);
    ~FramelessDialog() override = default;

signals:

    void positionChanged(const QPoint &newPosition);

protected:

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    bool eventFilter(QObject *watched, QEvent *event) override;

private:

    bool m_dragging = false;
    QPoint m_dragStartPos;
    QPoint m_windowStartPos;
};

#endif // FRAMELESSDIALOG_H
