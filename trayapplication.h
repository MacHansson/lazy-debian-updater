#ifndef TRAYAPPLICATION_H
#define TRAYAPPLICATION_H

#include <traydialog.h>
#include <logdialog.h>

#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QPixmap>

class TrayApplication : public QObject
{
    Q_OBJECT

public:

    explicit TrayApplication(QString logDirectory, QObject *parent = nullptr);
    ~TrayApplication();

public slots:

    void setIcon(QString icon, int rotationInDegreesPerSec);
    void setMessage(QString message);
    void setStatus(int status);
    //void setDialogPosition(QPoint pos);
    void quit();

protected:

    QSystemTrayIcon *trayIcon           {nullptr};
    TrayDialog *dlgTray                 {nullptr};
    LogDialog *dlgLog                   {nullptr};
    QString lastIcon                    {""};
    QString lastIconFilePath            {""};
    QString lastMessage                 {""};
    qreal stepAngleInDegrees            {0.0};
    qreal currentAngleInDegrees         {0.0};
    QTimer *timerRotate                 {nullptr};
    const qreal framesPerSecRotation    {30.0};

    QPixmap rotate(const QString &svgPath, qreal angleDegrees);

signals:

    void refresh();
    void trayDialogPositionChanged(QPoint pos);
    void quitApplication();

};

#endif // TRAYAPPLICATION_H
