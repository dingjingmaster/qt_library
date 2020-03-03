TEMPLATE = lib
CONFIG += plugin
TARGET = animal

DEFINES += BIL_LIB


LIBS += \
    -L ../build1 \
    -lbil

SOURCES += \
    ../bildog.cpp \
    ../dog.cpp

HEADERS += \
    ../bildog.h \
    ../dog.h

