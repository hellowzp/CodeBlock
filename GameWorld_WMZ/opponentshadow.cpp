#include "opponentshadow.h"
#include "opponent.h"
#include "mainwindow.h"

OpponentShadow::OpponentShadow(int type, int id, const int &x, const int &y, int healthPercentage):Opponent(type, id, x, y, healthPercentage)
{

}

OpponentShadow::~OpponentShadow()
{

}

void OpponentShadow::moveToPoint(int x, int y)
{
    this->setXPos(x);
    this->setYPos(y);
    this->setPos(this->getXPos() * SCALE_FACTOR, this->getYPos() * SCALE_FACTOR);
}

void OpponentShadow::shadowInjure(int leftPercentage)
{
    this->setHealthPercentage(leftPercentage);
}



