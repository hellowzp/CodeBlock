#include "heroshadow.h"
#include "mainwindow.h"

HeroShadow::HeroShadow(int role, int id, const QString &name, int x, int y, int healthPercentage, int magicPercentage) : Hero(role, id, name, x, y, healthPercentage, magicPercentage)
{

}

HeroShadow::~HeroShadow()
{
    qDebug() << "dtor of HeroShadow";
}

void HeroShadow::moveToPoint(int x, int y)
{
    this->setXPos(x);
    this->setYPos(y);
    this->setPos(this->getXPos() * SCALE_FACTOR, this->getYPos() * SCALE_FACTOR);
}

void HeroShadow::shadowMagic(int withSword, int x, int y)
{
    if (!this->getMagic().isRunning()) {

        this->getMagic().releaseAt(withSword, x, y);
    }
}

void HeroShadow::shadowInjure(int leftPercentage)
{
    this->setHealthPercentage(leftPercentage);
}


