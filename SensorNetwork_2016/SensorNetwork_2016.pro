TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    lab7/file_creator.c \
    lab8/sensor_node.c \
    lab8/tcpsock.c \
    lab9/sbuffer.c

HEADERS += \
    lab7/config.h \
    lab8/config.h \
    lab8/tcpsock.h \
    lab9/sbuffer.h

