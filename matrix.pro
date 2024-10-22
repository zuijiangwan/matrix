QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat
QT += serialport
QT += bluetooth
greaterThan(QT_MAJOR_VERSION, 5): QT += core bluetooth widgets

HEADERS += header/mainwindow.h
HEADERS += header/define.h
HEADERS += header/package.h
HEADERS += header/serialdialog.h
HEADERS += header/serialmodule.h
HEADERS += header/bluetoothmodule.h
HEADERS += header/bluetoothdialog.h
HEADERS += header/savefilethread.h
HEADERS += header/usbreceive.h
HEADERS += header/usbdialog.h
HEADERS += header/usbmodule.h

SOURCES += source/main.cpp
SOURCES += source/usbreceive.cpp
SOURCES += source/package.cpp
SOURCES += source/usbdialog.cpp
SOURCES += source/usbmodule.cpp
SOURCES += source/mainwindow.cpp
SOURCES += source/serialdialog.cpp
SOURCES += source/serialmodule.cpp
SOURCES += source/bluetoothdialog.cpp
SOURCES += source/bluetoothmodule.cpp
SOURCES += source/savefilethread.cpp

FORMS += form/mainwindow.ui
FORMS += form/usbdialog.ui
FORMS += form/serialdialog.ui
FORMS += form/bluetoothdialog.ui

RESOURCES += resource.qrc

LIBS += -L$$PWD/header/ -lCyAPI
INCLUDEPATH += $$PWD/header/inc
DEPENDPATH += $$PWD/header/inc
