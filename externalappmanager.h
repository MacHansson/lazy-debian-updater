#ifndef EXTERNALAPPMANAGER_H
#define EXTERNALAPPMANAGER_H

#include <QObject>

class ExternalAppManager : public QObject
{
    Q_OBJECT

    enum App {
        xterm,
        bash
    };

public:

    explicit ExternalAppManager(QObject *parent = nullptr);

    QString path(App app) {
        switch(app) {
            case xterm: return "/usr/bin/xterm";
            case bash:  return "/usr/bin/bash";
            default:    return QString();
        }
    }

public slots:

    void runExternalApp(QString app, QString command);

private:

    QStringList args_xterm;

};

#endif // EXTERNALAPPMANAGER_H
