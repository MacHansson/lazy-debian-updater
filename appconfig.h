#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QObject>
#include <QMap>
class AppConfig : public QObject
{
    Q_OBJECT

public:

    explicit AppConfig(QString filePath, QObject *parent = nullptr);
    QString getValue(QString key);

private:

    QMap<QString, QString> config;
};

#endif // APPCONFIG_H
