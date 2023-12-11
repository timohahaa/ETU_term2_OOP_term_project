QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    connection/client/qgameclient.cpp \
    game/cell.cpp \
    game/field.cpp \
    game/gamecontroler.cpp \
    main.cpp \
    mainwindow.cpp \
    connection/server/server.cpp \
    ui/qfieldwidget.cpp

HEADERS += \
    connection/client/qgameclient.h \
    connection/message.h \
    game/cell.h \
    game/field.h \
    game/gamecontroler.h \
    mainwindow.h \
    connection/server/server.h \
    ui/qfieldwidget.h
FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
