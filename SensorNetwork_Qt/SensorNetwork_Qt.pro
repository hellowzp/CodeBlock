TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    clientsensor.c \
    data_server.c \
    gateway.c \
    linkedlist.c \
    log-server.c \
    my_queue.c \
    statistic.c \
    tcpsocket.c \
    thread.c \
    queue.c \
    list.c \
    sensor_db.c \
    sensor_node.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    config.h \
    gateway.h \
    linkedlist.h \
    my_queue.h \
    statistic.h \
    tcpsocket.h \
    thread.h \
    queue.h \
    list.h \
    util.h \
    sensor_db.h

OTHER_FILES += \
    makefile \
    db.sql

