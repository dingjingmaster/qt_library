TARGET = Server
TEMPLATE = app

QT       += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    company.cpp


HEADERS  += \
    mainwindow.h \
    company.h

FORMS    += mainwindow.ui
