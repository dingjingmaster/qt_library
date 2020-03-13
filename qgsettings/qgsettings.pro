TEMPLATE = app
TARGET = qgsettings-demo

QT += core gui
CONFIG += qt no_keywords link_pkgconfig create_pc create_prl no_install_prl c++11

PKGCONFIG += gio-2.0

HEADERS = \
    $$PWD/QGSettings/qconftype.h \
    $$PWD/QGSettings/qgsettings.h \
    testkey.h

SOURCES = \
    $$PWD/main.cpp \
    $$PWD/QGSettings/qconftype.cpp \
    $$PWD/QGSettings/qgsettings.cpp \
    testkey.cpp
