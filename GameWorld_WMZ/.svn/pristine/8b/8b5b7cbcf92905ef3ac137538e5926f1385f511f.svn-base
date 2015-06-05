#include "hero.h"
#include <iostream>
#include "mainwindow.h"
#include <QTransform>
#include "opponent.h"
#include <QApplication>
#include <QGraphicsTextItem>


Hero::Hero(int role, int id, const QString &name, int x, int y, int healthPercentage, int magicPercentage):Tile(x, y, 0)
{

    this->_id = id;
    this->_role = role;

    this->_healthBar = new StatusBar(StatusBar::HealthBar, this);
    this->_healthBar->setTransform(QTransform().translate(0, -12));
    this->_healthBar->setPercentage(healthPercentage);
    this->_magicBar = new StatusBar(StatusBar::MagicBar, this);
    this->_magicBar->setTransform(QTransform().translate(0, -5));
    this->_magicBar->setPercentage(magicPercentage);

    this->setPos(this->getXPos() * SCALE_FACTOR, this->getYPos() * SCALE_FACTOR);


    if (role == PLAYER_ROLE_MAGE) {

        this->setPixmap(QPixmap(":/world/angry-bird-yellow-icon.png"));
    } else if (role == PLAYER_ROLE_PRIEST) {

        this->setPixmap(QPixmap(":/world/angry-bird-green-icon.png"));
    } else if (role == PLAYER_ROLE_WORRIOR) {

        this->setPixmap(QPixmap(":/world/angry-bird-icon.png"));
    }


    //set up the name label
    _nameLabel = new QGraphicsTextItem(name,this);
    _nameLabel->setDefaultTextColor(QColor(255, 0, 0));
    QFont font;
    font.setBold(true);
    _nameLabel->setFont(font);
    _nameLabel->setTransform(QTransform().translate(0, 24));


}

Hero::~Hero()
{
    qDebug() << "dtor of Hero";

    delete _nameLabel;
    delete _healthBar;
    delete _magicBar;
}




void Hero::setWorld(WorldView *world)
{
    this->_world = world;
    this->_magic.setWorld(world);
}

HeroMagic &Hero::getMagic()
{
    return _magic;
}

void Hero::setHealthPercentage(int percentage)
{
    this->_healthBar->setPercentage(percentage);
}

void Hero::setMagicPercentage(int percentage)
{
    this->_magicBar->setPercentage(percentage);
}

int Hero::getRole() const
{
    return _role;
}



int Hero::getId()
{
    return _id;
}










