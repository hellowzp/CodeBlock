TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    connmgr.c \
    datamgr.c \
    file_creator.c \
    sbuffer.c \
    sensor_db.c \
    sensor_node.c

OTHER_FILES += \
    Sensor.db \
    Sensor.db-journal \
    gateway.log \
    Makefile

HEADERS += \
    config.h \
    connmgr.h \
    datamgr.h \
    errmacros.h \
    sbuffer.h \
    sensor_db.h \
    util.h

