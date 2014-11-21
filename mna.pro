#-------------------------------------------------
#
# Project created by QtCreator 2014-11-17T20:08:13
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = mna
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += \
    include

SOURCES += \
    src/main.cpp \
    src/matrix/matrix.cpp \
    src/circuits/element.cpp

HEADERS += \
    include/matrix/matrix.h \
    element.h \
    include/circuits/element.h

OTHER_FILES += \
    data/simples.net
