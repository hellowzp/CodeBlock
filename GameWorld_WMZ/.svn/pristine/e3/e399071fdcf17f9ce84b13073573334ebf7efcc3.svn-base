#include "heromagic.h"
#include "opponentreal.h"
#include <QtCore/QtMath>
#include <QColor>
#include "worldview.h"
#include <QObject>
#include "worldbase.h"
#include "hero.h"



HeroMagic::HeroMagic(Hero *hero): _hero{hero}
{

}

void HeroMagic::releaseAt(int withSword, int x, int y)
{
    _x = x;
    _y = y;


    if (_hero->getRole() != PLAYER_ROLE_PRIEST) {


        if (_hero->getRole() == PLAYER_ROLE_MAGE) {
            _injure = 220;
            _range = 6;
        }
        if (_hero->getRole() == PLAYER_ROLE_WORRIOR) {
             _injure = 140;
             _range = 4;
        }
        if (withSword) {
            _injure *= 1.5;
            _range *= 1.5;
        }

        if (!this->isRunning()){

            this->start();

            //set injure of opponents
            for (OpponentReal *op : this->_world->_opponents) {

                if (qFabs(op->getXPos() - x) < _range && qFabs(op->getYPos() - y) < _range) {
                    op->makeInjure(_injure);
                }
            }
        }
    } else {

        _heal = 200;
        _range = 6;

        if (withSword) {
            _heal *= 1.5;
            _range *= 1.5;
        }

        if (!this->isRunning()){

            this->start();

            HeroReal *hero = this->_world->_hero;
            if (qFabs(hero->getXPos() - x) < _range && qFabs(hero->getYPos() - y) < _range) {

                hero->healHero(_heal);
            }
        }
    }

}

void HeroMagic::setWorld(WorldView *world)
{

    this->_world = world;
}



void HeroMagic::run() {



    QVector<WorldBase *> targetBases;
    for (int i = -5; i < _range; i++) {
        for (int j = -5; j < _range; j++) {
            if ( (0 <= _x + i  &&  _x + i < this->_world->_world->getCols()) && ( 0 <=_y + j && _y + j < this->_world->_world->getRows()) ) {
                WorldBase *base = this->_world->_bases[(_y + j)* this->_world->_world->getCols() + _x + i];
                QObject::connect(this, SIGNAL(highlightBeglin(QColor)), base, SLOT(setHightlight(QColor)));
                QObject::connect(this, SIGNAL(highlightEnd()), base, SLOT(unsetHightlight()));
                targetBases.push_back(base);
            }
        }
    }

    int xInt = qFabs(_x - _hero->getXPos());
    int yInt = qFabs(_y - _hero->getYPos());
    float ratio = (float)yInt / xInt;


    for (int i = 0; i < ((xInt > yInt) ? xInt : yInt); i++) {

        int x;
        int y;
        if (ratio < 1) {
            x = i * (_x - _hero->getXPos())/xInt + _hero->getXPos();
            y = ratio *i * (_y - _hero->getYPos())/yInt + _hero->getYPos();
        } else {
            x = i / ratio * (_x - _hero->getXPos())/xInt + _hero->getXPos();
            y = i * (_y - _hero->getYPos())/yInt + _hero->getYPos();
        }

        if ( (0 <= x  &&  x < this->_world->_world->getCols()) && ( 0 <= y && y < this->_world->_world->getRows()) ) {

            WorldBase *base = this->_world->_bases[y* this->_world->_world->getCols() + x];

            if (!targetBases.contains(base)) {

                QObject::connect(this, SIGNAL(highlightBeglin(QColor)), base, SLOT(setHightlight(QColor)));
                QObject::connect(this, SIGNAL(highlightEnd()), base, SLOT(unsetHightlight()));
                targetBases.push_back(base);
            }
        }


    }


    for (int i = 4; i > 0; i--) {

        if (_hero->getRole() != PLAYER_ROLE_PRIEST) {
            emit highlightBeglin(QColor(_injure * 0.6, 0, 0));
        } else {
            emit highlightBeglin(QColor(0, _heal * 0.6, 0));
        }


        this->msleep(200);
        emit highlightEnd();
        this->msleep(200);


    }

    for (WorldBase *base : targetBases) {
        QObject::disconnect(this, SIGNAL(highlightBeglin(QColor)), base, SLOT(setHightlight(QColor)));
        QObject::disconnect(this, SIGNAL(highlightEnd()), base, SLOT(unsetHightlight()));

    }



}
