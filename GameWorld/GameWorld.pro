#-------------------------------------------------
#
# Project created by QtCreator 2014-10-30T14:47:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GameWorld
TEMPLATE = app

QMAKE_CXXFLAGS = -std=c++11

SOURCES += main.cpp\
    MainWindow.cpp \
    TileItem.cpp \
    WorldView.cpp \
    HeroSprite.cpp \
    EnemySprite.cpp \
    world.cpp \
    SpriteItem.cpp \
    MoveableSpriteItem.cpp \
    StaticSpriteItem.cpp \
    DisplayView.cpp \
    ControlView.cpp

HEADERS  += \
    world.h \
    TileItem.h \
    MainWindow.h \
    WorldView.h \
    HeroSprite.h \
    EnemySprite.h \
    MoveableSpriteItem.h \
    SpriteItem.h \
    StaticSpriteItem.h \
    DisplayView.h \
    ControlView.h \
    PriorityQueue.h

FORMS    +=

OTHER_FILES += \
    GameWorld.pro.user

RESOURCES += \
    images.qrc
