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
    thread.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    config.h \
    gateway.h \
    linkedlist.h \
    my_queue.h \
    statistic.h \
    tcpsocket.h \
    thread.h

OTHER_FILES += \
    makefile

