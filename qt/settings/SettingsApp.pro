TEMPLATE = app
TARGET = SettingsApp
CONFIG += c++17

QT += core gui widgets

SOURCES += main.cpp \
           SettingsApp.cpp \
           SystemInfoPane.cpp

HEADERS += SettingsApp.h

DEFINES += QT_DEPRECATED_WARNINGS
