#include "externalappmanager.h"
#include <QProcess>

#include <debugflag.h>

ExternalAppManager::ExternalAppManager(QObject *parent)
    : QObject{parent}
{
    args_xterm << "-fa" << "Monospace" << "-fs" << "14" << "-e" << "bash" << "-c";
}

void ExternalAppManager::runExternalApp(QString app, QString command)
{
    if(command.isEmpty()) {
        if(DEBUG) qWarning() << "WARNING: Received an empty request:" << app;
        return;
    }

    if(app == "xterm") {
        args_xterm << command;
        QProcess::startDetached(path(App::xterm), args_xterm);
        return;
    }

    if(app == "bash") {
        QProcess::startDetached(path(App::bash), QStringList() << command);
        return;
    }

    if(DEBUG) qWarning() << "WARNING: Received an unknown request for an external app:" << app << command;
}
