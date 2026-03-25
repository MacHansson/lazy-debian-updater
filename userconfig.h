#ifndef USERCONFIG_H
#define USERCONFIG_H

#include <QObject>
#include <QPoint>

class UserConfig : public QObject
{
    Q_OBJECT

public:

    explicit UserConfig(QString filePath, QObject *parent = nullptr);

    void setTheme(int theme);
    int getTheme();

private:

    QString m_filePath {""};

};

#endif // USERCONFIG_H
