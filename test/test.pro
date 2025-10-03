QT       += core sql testlib

QT       -= gui
CONFIG  += c++14

TARGET    = test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    testturso.cpp

HEADERS += \
    testturso.h
