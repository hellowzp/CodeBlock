#include "opponentreal.h"
#include <QMutex>
#include <QDebug>
#include <QtCore/QtMath>
#include "hero.h"
#include "mainwindow.h"


OpponentReal::OpponentReal(int type, int id, const int &x, const int &y, int healthPercentage): Opponent(type, id, x ,y, healthPercentage)
{

    if (type == 1) {

        _totalHealth = 1000;
        _speed = 4;
    } else if (type == 2) {

        _totalHealth = 1500;
        _speed = 5;
    } else if (type == 3) {

        _totalHealth = 2000;
        _speed = 6;
    }

    _currentHealth = _totalHealth * healthPercentage / 100;

    QObject::connect(this, SIGNAL(positionChanged()), this, SLOT(refreshPosition()));
    _isAlive = true;
    _isPaused = false;

    _targetHero = nullptr;

    _preX = x;
    _preY = y;
}

OpponentReal::~OpponentReal()
{
    qDebug() << "dtor of OpponentReal";

    if (this->isRunning()) this->terminate();
}

void OpponentReal::activate() {


    this->start();
}

void OpponentReal::deactivate() {

    this->terminate();
}

void OpponentReal::redirectTo(int x, int y) {


    this->_pathFindingLocker.lock();
    this->_finder.findPath(this->getXPos(), this->getYPos(), x, y);
    this->_pathFindingLocker.unlock();
}

void OpponentReal::directToTarget()
{
    this->_pathFindingLocker.lock();
    this->_finder.findPath(this->getXPos(), this->getYPos(), this->_targetHero->getXPos(), this->_targetHero->getYPos());
    this->_pathFindingLocker.unlock();
}

bool OpponentReal::makeInjure(int value)
{
    if (!_isAlive) return false;

    _currentHealth -= value;

    if (_currentHealth <= 0) {

        this->terminate();
        _currentHealth = 0;
        _isAlive = false;

    }

    int percentage = _currentHealth * 100 / _totalHealth;


    qDebug() << "injureingainfsdgiandsf " << percentage;
    this->setHealthPercentage(percentage);
    emit opponentInjured(this->getId(), percentage);

    return false;
}

void OpponentReal::setWorld(WorldView *world)
{
    this->_world = world;
    this->_finder.setWorld(world);
    this->_analyzer.setWorld(world);
}

Hero *OpponentReal::getTargetHero()
{
    return _targetHero;
}

void OpponentReal::setTargetHero(Hero *hero)
{
    this->_targetHero = hero;
}

void OpponentReal::setPause(bool pause)
{
    _isPaused = pause;

}


void OpponentReal::refreshPosition()
{
    this->setPos(QPoint(this->getXPos() * SCALE_FACTOR, this->getYPos() * SCALE_FACTOR));

    emit opponentMoved(this->getId(), this->getXPos(), this->getYPos());
}

void OpponentReal::run()
{

    while(1) {
        while (this->_finder._pathNodes.size() > 0) {

            if (_isPaused) break;

            this->_pathFindingLocker.lock();
            PathNode *node = this->_finder._pathNodes.last();
            this->_finder._pathNodes.pop_back();
            this->_pathFindingLocker.unlock();

            this->makeInjure(_analyzer.valueDifference(_preX, _preY, node->x, node->y) * STEP_COST);

            this->setXPos(node->x);
            this->setYPos(node->y);

            _preX = node->x;
            _preY = node->y;



            emit positionChanged();

            this->msleep(250 + 1000 / _speed);


            if (this->_finder._pathNodes.size() < 4) {
                if (_targetHero != nullptr)
                    emit injureHero(this->_targetHero->getId(), 20);
            }

        }
        this->msleep(250 + 1000 / _speed);

        if (_isPaused) continue;  // skip the next operations when paused;

        if (_targetHero != nullptr)
            emit injureHero(this->_targetHero->getId(), 20);
    }



}
