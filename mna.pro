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
    src/mna.cpp \
    src/matrix/solve.cpp \

HEADERS += \
    include/matrix/solve.h

OTHER_FILES += \
    data/simples.net
