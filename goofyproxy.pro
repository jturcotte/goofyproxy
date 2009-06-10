# -------------------------------------------------
# Project created by QtCreator 2009-05-30T08:29:52
# -------------------------------------------------
QT -= gui
QT += network
TARGET = goofyproxy
CONFIG += console

CONFIG -= app_bundle
TEMPLATE = app
HEADERS = main.h \
    httpforwardjob.h \
    httpforwardjobthread.h \
    ahttpfilter.h \
    defaultfilter.h \
    cmdpipefilter.h
SOURCES += main.cpp \
    httpforwardjob.cpp \
    httpforwardjobthread.cpp \
    ahttpfilter.cpp \
    defaultfilter.cpp \
    cmdpipefilter.cpp
