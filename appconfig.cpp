#include "appconfig.h"
#include <QFile>

#include <debugflag.h>

#include <debugflag.h>

AppConfig::AppConfig(QString filePath, QObject *parent)
    : QObject{parent}
{
    QFile file(filePath);
    bool open = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(DEBUG) qDebug() << "Opening app config file" << filePath << "with result" << open;
    while(true) {
        QStringList temp = QString(file.readLine()).replace("\n", "").replace("\r","").split("=");
        if(temp.size() == 2) {
            config.insert(temp.at(0), temp.at(1));
        } else {
            break;
        }
    }
    for(auto i=config.begin(), iEnd = config.end(); i != iEnd; ++i) {
        //qDebug() << "outer loop" << i.key();
        for(auto k=config.begin(), kEnd = config.end(); k != kEnd; ++k) {
            //qDebug() << "inner loop" << k.key();
            if(k.value().contains("$" + i.key())) {
                //qDebug() << k.value() << "contains" << i.key() << " = " << k.value().contains("$" + i.key());
                config.insert(k.key(), QString(k.value()).replace("$" + i.key(), i.value()));
            }
        }
    }
    if(DEBUG) {
        for(auto i=config.begin(), end = config.end(); i != end; ++i) {
            qDebug() << i.key() + " = " + i.value();
        }
    }
}

QString AppConfig::getValue(QString key)
{
    return config.value(key);
}
