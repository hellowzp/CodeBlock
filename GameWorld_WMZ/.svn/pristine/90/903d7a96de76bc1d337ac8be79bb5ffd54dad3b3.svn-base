#include "worldbase.h"
#include <iostream>


WorldBase::WorldBase(const Tile & tile): Tile(tile){

    this->setRect(this->getXPos() * SCALE_FACTOR, this->getYPos() * SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);

    this->_color = QColor(0, 255 * this->getValue(), 0);
    this->setBrush(QBrush(this->_color));
}

WorldBase::~WorldBase()
{
    qDebug() << "dtor of WorldBase";
}

void WorldBase::setHightlight(const QColor &color)
{
    //std::cout << "hight received" << std::endl;
    this->setBrush(QBrush(color));
}

void WorldBase::unsetHightlight()
{
    this->setBrush(QBrush(this->_color));
}


void WorldBase::setColor(int color) {

    if (color == 1) {

        this->_color = QColor(255 * this->getValue(), 0, 0);

    } else if (color == 2) {

        this->_color = QColor(0, 255 * this->getValue(), 0);

    } else if (color == 3) {

        this->_color = QColor(0, 0, 255 * this->getValue());
    }

    this->setBrush(QBrush(this->_color));
}


