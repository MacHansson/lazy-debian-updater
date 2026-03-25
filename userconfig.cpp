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

int UserConfig::getTheme()
{
    QSettings settings(QDir::toNativeSeparators(m_filePath), QSettings::IniFormat);
    int theme = settings.value("theme", "default").toInt();

    if(DEBUG) qDebug() << "Read setting: key theme" << theme;
    return theme; // Return needed to avoid warning - remove this line if you uncomment the function code above
}

void UserConfig::setTheme(int theme)
{
    QSettings settings(QDir::toNativeSeparators(m_filePath), QSettings::IniFormat);
    settings.setValue("theme", theme);

    if(DEBUG) qDebug() << "Write setting: key theme" << "value" << theme;
}
