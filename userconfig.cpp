#include "userconfig.h"
#include <QScreen>
#include <QSettings>
#include <QDir>

#include <debugflag.h>

UserConfig::UserConfig(QString filePath, QObject *parent)
    : QObject{parent}, m_filePath{filePath}
{
    // nothing to do here
}

QPoint UserConfig::getTrayDialogPosition()
{
    /*QScreen *p = QGuiApplication::primaryScreen();
    QString display = p->manufacturer() + "-" + p->model() + "-" + p->serialNumber();
    display.remove(" ").remove(".");
    QSettings settings(QDir::toNativeSeparators(m_filePath), QSettings::IniFormat);
    settings.beginGroup("Display");
    QPoint pos = settings.value(display, QPoint(0, 0)).toPoint();
    settings.endGroup();

    if(DEBUG) qDebug() << "Read setting: key" << display << "value" << pos;
    return pos;*/
    return QPoint(); // Return needed to avoid warning - remove this line if you uncomment the function code above
}

void UserConfig::setTrayDialogPosition(QPoint pos)
{
    Q_UNUSED(pos)
    /*QScreen *p = QGuiApplication::primaryScreen();
    QString display = p->manufacturer() + "-" + p->model() + "-" + p->serialNumber();
    display.remove(" ").remove(".");
    QSettings settings(QDir::toNativeSeparators(m_filePath), QSettings::IniFormat);
    settings.beginGroup("Display");
    settings.setValue(display, pos);
    settings.endGroup();

    if(DEBUG) qDebug() << "Write setting: key" << display << "value" << pos;*/
}
