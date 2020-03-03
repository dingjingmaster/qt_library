QT -= gui
QT += network

#CONFIG += c++11 console
CONFIG -= app_bundle
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        httpserver.cpp \
        main.cpp

HEADERS += \
    httpserver.h
