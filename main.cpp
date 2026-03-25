#include <QApplication>
#include <QDir>

#include <debugflag.h>
#include <filewatcher.h>
#include <trayapplication.h>
#include <externalappmanager.h>
#include <userconfig.h>
#include <appconfig.h>

bool DEBUG {false};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    if(QApplication::applicationDirPath().contains("build")) {
        qDebug() << "Detecting start from within the build-directory, activating debugging ...";
        DEBUG = true;
    }

    ExternalAppManager extAppMan;
    AppConfig appConfig(QApplication::applicationDirPath() + "/config");
    UserConfig userConfig(QDir::homePath() + "/.config/debie");

    TrayApplication trayApp(&userConfig, appConfig.getValue("LOGDIR"));
    QObject::connect(&trayApp, &TrayApplication::quitApplication, [&]() { QApplication::quit(); });
    QObject::connect(&trayApp, &TrayApplication::refresh, &trayApp, [&]() {
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
