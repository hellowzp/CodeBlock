TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += gateway.c \
    tcpsocket.c \
    queue.c \
    list.c \
    sensor_db.c \
    sensor_node.c \
    main.c \
    test.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    config.h \
    tcpsocket.h \
    queue.h \
    list.h \
    util.h \
    sensor_db.h

OTHER_FILES += \
    makefile \
    db.sql

