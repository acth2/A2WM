TEMPLATE = app
TARGET = a2wm
CONFIG += console c++11
CONFIG += qt
CONFIG += release
CONFIG += no_accessibility
CONFIG += no_pro
CONFIG += c++17
QT += core gui widgets

LIBS += -lX11
SOURCES += main.cpp \
           windowmanager.cpp \
           konami_code_handler.cpp \
           userinteractright.cpp \
           taskbar.cpp \
           win/topbar.cpp \
           win/MinimizedPosArray.cpp
           
HEADERS += windowmanager.h \
           konami_code_handler.h \
           userinteractright.h \
           taskbar.h \
           win/topbar.h \
           win/MinimizedPosArray.h

DESTDIR = src

INSTALLS += target
target.path = /usr/bin
target.files = $$DESTDIR/a2wm
