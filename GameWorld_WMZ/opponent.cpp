#include "opponent.h"
#include <iostream>
#include "mainwindow.h"

#include <QMutex>
#include <QThread>
#include "statusbar.h"


Opponent::Opponent(int type, int id, const int &x, const int &y, int healthPercentage):Enemy(x, y, 0) {

    _type = type;
    _id = id;

    this->setX(this->getXPos() * SCALE_FACTOR);
    this->setY(this->getYPos() * SCALE_FACTOR);

    if (_type == 1) {

        this->setPixmap(QPixmap(":/world/Pig-icon.png").scaled(40, 40));
    } else if (_type == 2) {

        this->setPixmap(QPixmap(":/world/Pig-icon.png").scaled(45, 45));
    } else if (_type == 3) {

        this->setPixmap(QPixmap(":/world/Pig-icon.png").scaled(50, 50));
    }

    this->_healthBar = new StatusBar(StatusBar::HealthBar, this);
    this->_healthBar->setPercentage(healthPercentage);

}

Opponent::~Opponent()
{
    qDebug() << "dtor of Opponent";
    delete _healthBar;
}

int Opponent::getId()
{
    return _id;
}

void Opponent::setHealthPercentage(int percentage)
{

    _healthBar->setPercentage(percentage);

}


