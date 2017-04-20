#-------------------------------------------------
#
# Project created by QtCreator 2017-04-19T18:30:51
#
#-------------------------------------------------

QT       -= core gui

TARGET = ../lib/libdsvls
TEMPLATE = lib
INCLUDEPATH = ../include
OBJECTS_DIR = build/

DEFINES += LIBDSVLS_LIBRARY

SOURCES += src/*

HEADERS += src/*
HEADERS += ../include


#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
