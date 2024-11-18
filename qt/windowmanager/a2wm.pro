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
SOURCES += main.cpp \
           windowmanager.cpp \
           userinteractright.cpp \
           taskbar.cpp \
           mousemover.cpp
           
HEADERS += windowmanager.h \
           userinteractright.h \
           taskbar.h \
           utils/ClickableLabel.h \
           mousemover.h

DESTDIR = src

INSTALLS += target
target.path = /usr/bin
target.files = $$DESTDIR/a2wm

//QMAKE_POST_LINK += mkdir -p /usr/cydra/info /usr/cydra/settings /usr/cydra/logs /usr/cydra/icons /usr/cydra/backgrounds; \
//                   touch /usr/cydra/logs/a2wm.log; \
//                   chmod 777 /usr/cydra/logs/a2wm.log; \
//                   cp -r win/cydra.png /usr/cydra; \
//                   cp -r win/cydradm.png /usr/cydra; \
//                   cp -r win/power.png /usr/cydra/icons; \
//                   cp -r win/usrLogo.png /usr/cydra/icons
