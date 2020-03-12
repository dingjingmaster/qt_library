TARGET = Client
TEMPLATE = app

QT       += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    companyInterface.cpp \
    candidate.cpp

HEADERS  += \
    mainwindow.h \
    companyInterface.h \
    candidate.h

FORMS    += mainwindow.ui

DISTFILES += \
    README.md
