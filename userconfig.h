#ifndef USERCONFIG_H
#define USERCONFIG_H

#include <QObject>
#include <QPoint>

class UserConfig : public QObject
{
    Q_OBJECT

public:

    explicit UserConfig(QString filePath, QObject *parent = nullptr);

    QPoint getTrayDialogPosition();             // deprecated - position of tray dialog ist set automatically
    void setTrayDialogPosition(QPoint pos);     // deprecated - position of tray dialog ist set automatically

private:

    QString m_filePath {""};

};

#endif // USERCONFIG_H
