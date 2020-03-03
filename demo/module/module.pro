QT -= gui

CONFIG += c++11
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

LIBS += \
        -Lplugin/build1/ \
        -lbil \

INCLUDEPATH += \
        plugin/

#DEFINES += BIL_LIB

HEADERS += \

SOURCES += \
        main.cpp

DISTFILES += \
    README.md

