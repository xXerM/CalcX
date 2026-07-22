QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = CalcX
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/calculator.cpp \
    src/themeeditor.cpp \
    src/themesettings.cpp

HEADERS += \
    src/calculator.h \
    src/themesettings.h \
    src/themeeditor.h

RESOURCES += \
    CalcX.qrc

target.path = /usr/bin
INSTALLS += target
