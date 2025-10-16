#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QObject>
#include <QFileSystemWatcher>

class FileWatcher : public QObject
{
    Q_OBJECT

public:

    explicit FileWatcher(QString watchedFile, QString svgDir, QObject *parent = nullptr);

protected:

    QFileSystemWatcher watcher;
    QString m_svgDir;

    QString readFirstLine(const QString& filePath);
    void interpret(QString line);

signals:

    void iconChanged(QString newIcon, int rotationInDegreesPerSec);
    void messageChanged(QString message);
    void statusChanged(int status);
    void runExternalAppRequested(QString app, QString command);
    void receivedQuit();

};

#endif // FILEWATCHER_H
