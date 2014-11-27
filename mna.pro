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
    src/circuits/circuit.cpp \
    src/circuits/element.cpp

HEADERS += \
    include/consts.h \
    include/utils.h \
    include/matrix/matrix.h \
    include/circuits/circuit.h \
    include/circuits/element.h

OTHER_FILES += \
    data/simples.net \
    data/simples_tran.net \
    data/simples_poly.net
