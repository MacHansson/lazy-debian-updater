#include "filewatcher.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QPalette>

#include <debugflag.h>

FileWatcher::FileWatcher(QString watchedFile, QString svgDir, QObject *parent)
    : QObject{parent}, m_svgDir{QDir::toNativeSeparators(svgDir)}
{
    QFile file(QDir::toNativeSeparators(watchedFile));
    if(!file.exists()) {
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        file.close();
    }
    else {
        file.resize(0);
    }

    watcher.addPath(file.fileName());
    QObject::connect(&watcher, &QFileSystemWatcher::fileChanged, [&](const QString &path) {
        QString line = readFirstLine(path);
        if(!line.isEmpty()) {
            interpret(line);
        }
        watcher.addPath(path); // Add again if removed, happens sometimes
    });
}

QString FileWatcher::readFirstLine(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QString line = file.readLine();
    file.close();

    return line.remove("\r").remove("\n").trimmed();
}

void FileWatcher::interpret(QString line)
{
    if(line.contains(";")) { // Allow multiple commands in one line and process them separately
        QStringList list = line.split(";");
        for(QString entry : list) {
            interpret(entry);
        }
        return;
    }
    if(DEBUG) qDebug() << "Command in temporary file received:" << line;

    QString prefix = QString();
    QString content = QString();
    QStringList options = QStringList(); // Option list prepared for future changes
    if(line.contains(":")) {
        options = line.split(":");
        prefix = options.takeFirst();
        content = options.takeFirst();
    } else {
        content = line;
    }

    if(content.isEmpty()) {
        if(DEBUG) qWarning() << "WARNING: Empty content received in request:" << line;
        return;
    }

    if(prefix == "s") {
        emit statusChanged(content.toInt());
        return;
    }

    if(prefix == "i"){
        QString icon = QDir(m_svgDir).filePath(content);

        emit iconChanged(icon, options.isEmpty() ? 0 : options.at(0).toInt());
        return;
    }

    if(prefix == "m") {
        emit messageChanged(content);
        return;
    }

    if(prefix == "c") {
        if(options.isEmpty()) {
            if(DEBUG) qWarning() << "WARNING: Command request without options received:" << content;
        }
        for(QString option : options) {
            emit runExternalAppRequested(content, option);
        }
    }

    if(content == "q") {
        emit receivedQuit();
        return;
    }

    if(DEBUG) qWarning() << "WARNING: Unknown file content detected:" << line;
}
