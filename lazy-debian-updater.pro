QT       += core gui widgets svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    appconfig.cpp \
    externalappmanager.cpp \
    filewatcher.cpp \
    framelessdialog.cpp \
    logdialog.cpp \
    main.cpp \
    trayapplication.cpp \
    traydialog.cpp \
    userconfig.cpp

HEADERS += \
    appconfig.h \
    debugflag.h \
    externalappmanager.h \
    filewatcher.h \
    framelessdialog.h \
    logdialog.h \
    trayapplication.h \
    traydialog.h \
    userconfig.h

FORMS += \
    logdialog.ui \
    traydialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    svg.qrc
