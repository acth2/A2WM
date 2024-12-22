TEMPLATE = app
TARGET = a2wm
CONFIG += console c++11
CONFIG += qt
CONFIG += release
CONFIG += no_accessibility
CONFIG += no_pro
CONFIG += c++17
QT += core gui widgets x11extras

LIBS += -lX11
LIBS += -lXtst
SOURCES += main.cpp \
           windowmanager.cpp \
           userinteractright.cpp \
           taskbar.cpp
           
HEADERS += windowmanager.h \
           userinteractright.h \
           taskbar.h \
           utils/ClickableLabel.h

DESTDIR = src

INSTALLS += target
target.path = /usr/bin
target.files = $$DESTDIR/a2wm
