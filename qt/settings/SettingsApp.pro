TEMPLATE = app
TARGET = SettingsApp
CONFIG += c++17

QT += core gui widgets

SOURCES += main.cpp \
           SettingsApp.cpp

HEADERS += SettingsApp.h \
           SystemInfoPane.h

DEFINES += QT_DEPRECATED_WARNINGS
