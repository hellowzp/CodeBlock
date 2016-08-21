TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    file_creator.c \
    lib/tcpsock.c \
    sensor_node.c \
    connmgr.c \
    datamgr.c \
    sbuffer.c \
    sensor_db.c \
    lib/dplist.c

HEADERS += \
    config.h \
    connmgr.h \
    datamgr.h \
    sbuffer.h \
    sensor_db.h \
    lib/dplist.h \
    lib/tcpsock.h \
    errutil.h \
    util.h

OTHER_FILES += \
    Makefile

