#include <QApplication>
#include <QDir>

#include <debugflag.h>
#include <filewatcher.h>
#include <trayapplication.h>
#include <externalappmanager.h>
#include <userconfig.h>
#include <appconfig.h>

bool DEBUG = true;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ExternalAppManager extAppMan;
    AppConfig appConfig(QApplication::applicationDirPath() + "/config");
    UserConfig userConfig(QDir::homePath() + "/.config/debie");

    TrayApplication trayApp(appConfig.getValue("LOGDIR"));
    //trayApp.setDialogPosition(userConfig.getTrayDialogPosition());
    QObject::connect(&trayApp, &TrayApplication::quitApplication, [&]() { QApplication::quit(); });
    //QObject::connect(&trayApp, &TrayApplication::trayDialogPositionChanged, [&](QPoint pos) { userConfig.setTrayDialogPosition(pos); });
    QObject::connect(&trayApp, &TrayApplication::refresh, [&]() {
        if(!DEBUG) {
            extAppMan.runExternalApp("bash", QApplication::applicationDirPath() + "/run.sh");
        } else {
            extAppMan.runExternalApp("bash", "/usr/local/sbin/run.sh");
        }
    });

    FileWatcher fileWatcher(appConfig.getValue("TEMPFILE"), ":/resources/svg/");
    QObject::connect(&fileWatcher, &FileWatcher::iconChanged, &trayApp, &TrayApplication::setIcon);
    QObject::connect(&fileWatcher, &FileWatcher::messageChanged, &trayApp, &TrayApplication::setMessage);
    QObject::connect(&fileWatcher, &FileWatcher::statusChanged, &trayApp, &TrayApplication::setStatus);
    QObject::connect(&fileWatcher, &FileWatcher::receivedQuit, &trayApp, &TrayApplication::quit);
    QObject::connect(&fileWatcher, &FileWatcher::runExternalAppRequested, &extAppMan, &ExternalAppManager::runExternalApp);

    return app.exec();
}
