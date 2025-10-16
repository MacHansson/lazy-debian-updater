#include "trayapplication.h"
#include <QApplication>
#include <QSvgRenderer>
#include <QPainter>
#include <QTransform>
#include <QMenu>

#include <debugflag.h>

TrayApplication::TrayApplication(QString logDirectory, QObject *parent)
    : QObject{parent}
{
    trayIcon = new QSystemTrayIcon(QIcon(""));

    dlgTray = new TrayDialog();
    QObject::connect(dlgTray, &TrayDialog::positionChanged, [&](QPoint pos) {
        emit trayDialogPositionChanged(pos);
    });
    QObject::connect(dlgTray, &TrayDialog::themeChanged, [&]() {
        setIcon(lastIcon, currentAngleInDegrees);
    });
    QObject::connect(dlgTray, &TrayDialog::refresh, [&]() {
        emit refresh();
    });

    dlgLog = new LogDialog(logDirectory);
    QObject::connect(dlgTray, &TrayDialog::showLogDialog, [&]() {
        dlgLog->updateLogs();
        dlgLog->show();
    });

    timerRotate = new QTimer();
    timerRotate->setInterval(1000 / framesPerSecRotation);
    timerRotate->setSingleShot(false);

    QObject::connect(trayIcon, &QSystemTrayIcon::activated, [&]() {
        if(!dlgTray->isVisible()) {
            QPoint globalCursorPos = QCursor::pos();
            QScreen *screen = qApp->screenAt(globalCursorPos);
            if(DEBUG) qDebug() << "Position:" << globalCursorPos << "Screen:" << screen->name() << "Available geometry:" << screen->availableGeometry();

            int x = globalCursorPos.x();
            int y = globalCursorPos.y();
            if(x < screen->availableGeometry().width() / 2) {
                x = 0;
            }
            else {
                x = screen->availableGeometry().width() - dlgTray->width();
            }
            if(y < screen->availableGeometry().height() / 2) {
                y = 0;
            }
            else {
                y = screen->availableGeometry().height() - dlgTray->height();
            }

            if(DEBUG) qDebug() << "Moving tray dialog to:" << x << y;
            dlgTray->move(x, y);

            dlgTray->show();
            dlgTray->raise();
            dlgTray->activateWindow();
        } else {
            dlgTray->hide();
        }
    });

    QObject::connect(timerRotate, &QTimer::timeout, [&]() {
        currentAngleInDegrees += stepAngleInDegrees;
        if(currentAngleInDegrees > 360.0) {
            currentAngleInDegrees -= 360.0;
        }
        trayIcon->setIcon(QIcon(rotate(lastIconFilePath, currentAngleInDegrees)));
    });

    if(DEBUG) {
        setIcon(":/resources/svg/working", 100);
        trayIcon->setVisible(true);
    }
}

TrayApplication::~TrayApplication()
{
    if(DEBUG) qDebug() << "Quit";
    trayIcon->deleteLater();
    dlgTray->deleteLater();
    dlgLog->deleteLater();
    timerRotate->deleteLater();
}

void TrayApplication::setIcon(QString icon, int rotationInDegreesPerSec)
{
    QString iconFilePath = icon + (dlgTray->darkMode() ? "-dark" : "-light") + ".svg";
    if (iconFilePath == lastIconFilePath) {
        if(DEBUG) qDebug() << "Doing nothing, icon already set:"  << iconFilePath;
        return;
    }

    trayIcon->setIcon(QIcon(iconFilePath));
    trayIcon->setVisible(true);
    lastIcon = icon;
    lastIconFilePath = iconFilePath;

    if(rotationInDegreesPerSec > 0) {
        stepAngleInDegrees = qreal(rotationInDegreesPerSec) / framesPerSecRotation;
        if(!timerRotate->isActive()) {
            timerRotate->start();
        }
    }
    else {
        currentAngleInDegrees = 0;
        timerRotate->stop();
    }

    if(DEBUG) qDebug() << "New icon set:"  << iconFilePath;
    return;
}

void TrayApplication::setMessage(QString message)
{
    lastMessage = message;
    dlgTray->setMessage(message);
    if(DEBUG) qDebug() << "New message set:"  << message;
}

void TrayApplication::setStatus(int status)
{
    dlgTray->setStatus(status);
    if(status == 0) {
        dlgLog->updateLogs();
        dlgTray->setLastUpdateDateTime(dlgLog->getLastLogDateTime());
    }
    if(DEBUG) qDebug() << "New status set:" << status;
}

/*void TrayApplication::setDialogPosition(QPoint pos)
{
    if(pos != QPoint(0, 0)) {
        dlgTray->move(pos);
    }
}*/

void TrayApplication::quit()
{
    trayIcon->deleteLater();
    dlgTray->deleteLater();
    emit quitApplication();
}

QPixmap TrayApplication::rotate(const QString& svgPath, qreal angleDegrees) {
    QSvgRenderer renderer(svgPath);
    QSize size = renderer.defaultSize();

    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QTransform transform;

    // Rotate around the center
    transform.translate(size.width() / 2.0, size.height() / 2.0);
    transform.rotate(angleDegrees);
    transform.translate(-size.width() / 2.0, -size.height() / 2.0);

    painter.setTransform(transform);
    renderer.render(&painter);

    return pixmap;
}
