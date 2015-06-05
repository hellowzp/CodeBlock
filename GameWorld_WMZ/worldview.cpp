#include "worldview.h"
#include <iostream>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QPixmap>
#include <QLabel>
#include <QDrag>
#include <QString>
#include <QGraphicsPixmapItem>
#include "mainwindow.h"
#include <QMimeData>
#include <QDragLeaveEvent>
#include <QGraphicsOpacityEffect>
#include "worldbase.h"
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include "worldpathfinder.h"
#include "opponent.h"
#include <QtCore/QtMath>
#include "gamehoster.h"
#include <QDebug>
#include "gameclient.h"
#include <QtCore/QtMath>
#include "opponentshadow.h"
#include "opponentreal.h"
#include <QMessageBox>
#include "gift.h"



WorldView::WorldView() {

}

WorldView::WorldView(const QString& url) {

    this->_world = new World;
    std::vector<Tile*> tiles = this->_world->createWorld(url);

    this->setScene(new QGraphicsScene);
    this->setFixedSize(this->_world->getCols() * SCALE_FACTOR, this->_world->getCols() * SCALE_FACTOR);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    for (int i = 0; i < this->_world->getRows(); i++) {
        for (int j = 0; j < this->_world->getCols(); j++) {

            WorldBase *base = new WorldBase(*tiles[ i * this->_world->getCols() + j]);
            base->setZValue(1);
            this->scene()->addItem(base);
            this->_bases.push_back(base);
        }
    }

    this->_hero = nullptr;
    _isPaused = false;

}

WorldView::~WorldView()
{
    qDebug() << "dtor of WorldView";

    delete _hero;

    for (HeroShadow *shadow : _heroShadows) delete shadow;
    for (OpponentReal * op : _opponents) delete op;
    for (OpponentShadow *shadow : _opponentShadows) delete shadow;
    for (WorldBase *base : _bases) delete base;
}


void WorldView::mousePressEvent(QMouseEvent *event) {

    //std::cout << "pressed at " <<  event->x() << "  "<< event->y() << std::endl;

    event->accept();

    int x = event->pos().x();
    int y = event->pos().y();
    int xPosition = x / SCALE_FACTOR;
    int yPosition = y / SCALE_FACTOR;

    HeroReal *hero = this->_hero;

    Qt::MouseButtons mouseButtons = event->buttons();

    if( mouseButtons == Qt::LeftButton ) {


        hero->releaseMagicAt(xPosition, yPosition);


    }
    else if( mouseButtons == Qt::RightButton ){


        int dir = calculateHeroDir(xPosition, yPosition);
        hero->faceToDirection(dir);

        hero->startMoving();
    }


}

void WorldView::mouseMoveEvent(QMouseEvent *event) {


    if (rand() % 3 != 0) return;

    int x = event->pos().x();
    int y = event->pos().y();
    int xPosition = x / SCALE_FACTOR;
    int yPosition = y / SCALE_FACTOR;

    HeroReal *hero = this->_hero;
    int dir = calculateHeroDir(xPosition, yPosition);

    hero->faceToDirection(dir);
}

void WorldView::mouseReleaseEvent(QMouseEvent *event) {

    if (event->button() == Qt::LeftButton) {
        //qDebug("release left");
    }
    if (event->button() == Qt::RightButton) {
        //qDebug("release right");
        HeroReal *hero = this->_hero;
        hero->stopMoving();
    }
    event->accept();


}

void WorldView::setAssetsFrame(AssetsFrame *frame)
{
    this->_assetsFrame = frame;
}

void WorldView::setMainWindow(MainWindow *mainWindow)
{
    _mainWindow = mainWindow;
}

void WorldView::drawMagic()
{
    this->scene()->addEllipse(10,10,100,100, QPen(), QBrush(QColor(233,0,0)));
}

void WorldView::createHero(HeroModel hero)
{

    _hero = new HeroReal(hero.role, hero.id, hero.name, hero.x, hero.y, hero.healthPercentage, hero.magicPercentage);
    this->scene()->addItem(_hero);
    _hero->setWorld(this);
    _hero->setZValue(80);
    _hero->show();

    GameClient *client = GameClient::Instance();
    QObject::connect(_hero, SIGNAL(heroMoved(int,int,int)), client, SLOT(heroMoved(int,int,int)));
    QObject::connect(_hero, SIGNAL(heroMovingStoped(int)), client, SLOT(heroMovingStoped(int)));
    QObject::connect(_hero, SIGNAL(needToRepathOpponents(int)), this, SLOT(repathOpponents(int)));
    QObject::connect(_hero, SIGNAL(magicReleased(int,int,int,int)), client, SLOT(magicReleased(int,int,int,int)));
    QObject::connect(client, SIGNAL(needToInjureHero(int)), _hero, SLOT(injureHero(int)));
    QObject::connect(_hero, SIGNAL(heroInjured(int,int)), client, SLOT(heroInjured(int,int)));
    QObject::connect(_hero, SIGNAL(giftPicked(int, int, int)), client, SLOT(giftPicked(int, int, int)));
    QObject::connect(_hero, SIGNAL(heroHealthIncreased(int,int)), client, SLOT(heroHealthIncreased(int,int)));
    QObject::connect(_hero, SIGNAL(heroMagicIncreased(int,int)), client, SLOT(heroMagicIncreased(int,int)));
    QObject::connect(_hero, SIGNAL(heroMagicUsed(int,int)), client, SLOT(heroMagicUsed(int,int)));

    QObject::connect(_assetsFrame, SIGNAL(equipmentChanged(bool,bool,bool)), _hero, SLOT(equipmentChanged(bool,bool,bool)));
    QObject::connect(_assetsFrame, SIGNAL(drinkEnergy(int)), _hero, SLOT(drinkEnergy(int)));
    QObject::connect(_hero, SIGNAL(updateProgressBars(int,int,int)), _mainWindow, SLOT(updateProgressBars(int,int,int)));

    _hero->setProgressBars();

    //tell the client the hero has been created

    emit heroCreated(hero);
}

void WorldView::addHeroShadow(HeroModel hero)
{
    HeroShadow *shadow = new HeroShadow(hero.role, hero.id, hero.name, hero.x, hero.y, hero.healthPercentage, hero.magicPercentage);

    if (_heroShadows.size() > 0) {

        shadow->setZValue(_heroShadows.last()->zValue() + 1);
    } else {

        shadow->setZValue(10);
    }
    this->_heroShadows.push_back(shadow);
    this->scene()->addItem(shadow);
    shadow->setWorld(this);

    shadow->show();
}

void WorldView::addOpponentShadow(OpponentModel opponent)
{
    OpponentShadow *shadow = new OpponentShadow(opponent.type, opponent.id, opponent.x, opponent.y, opponent.healthPecentage);

    qDebug() << "adding a opponent shadow " << opponent.id;

    if (_opponentShadows.size() > 0) {

        shadow->setZValue(_opponentShadows.last()->zValue() + 1);
    } else {

        shadow->setZValue(20);
    }

    this->_opponentShadows.push_back(shadow);
    this->scene()->addItem(shadow);
    shadow->show();
}

void WorldView::moveShadow(int id, int x, int y)
{
    for (HeroShadow *shadow : _heroShadows) {

        if (shadow->getId() == id) {

            shadow->moveToPoint(x, y);
        }
    }

    for (OpponentShadow *shadow : _opponentShadows) {

        if (shadow->getId() == id) {

            shadow->moveToPoint(x, y);
        }
    }
}

void WorldView::createOpponent(int type, int id, int x, int y)
{
    OpponentModel opponentModel;
    opponentModel.type = type;
    opponentModel.id = id;
    opponentModel.x = x;
    opponentModel.y = y;
    opponentModel.healthPecentage = 100;

    qDebug() << "creating a opponent " << id;

    OpponentReal *opponent = new OpponentReal(type, id, x, y, 100);
    if (_opponents.size() > 0) {
        opponent->setZValue(_opponents.last()->zValue() + 1);
    } else {
        opponent->setZValue(20);
    }

    _opponents.push_back(opponent);
    this->scene()->addItem(opponent);
    opponent->show();
    opponent->setWorld(this);
    opponent->activate();

    this->repathOpponents(this->_hero->getId());

    GameClient *client = GameClient::Instance();
    QObject::connect(opponent, SIGNAL(opponentMoved(int,int,int)), client, SLOT(opponentMoved(int,int,int)));
    QObject::connect(opponent, SIGNAL(opponentInjured(int,int)), client, SLOT(opponentInjured(int,int)));
    QObject::connect(opponent, SIGNAL(injureHero(int,int)), client, SLOT(injureHero(int,int)));

    emit OpponentCreated(opponentModel);
}

void WorldView::repathOpponents(int heroId)
{

    for (OpponentReal *opponent : _opponents) {

        //check if there are nearer hero around then the current target


        if (_hero == nullptr && _heroShadows.size() == 0) return;

        if (!_hero->isAlive() && _heroShadows.size() == 0) return;

        int shortestX;
        int shortestY;
        Hero *shortestTarget;

        if (_hero != nullptr || (!_hero->isAlive())) {

            shortestX = _hero->getXPos();
            shortestY = _hero->getYPos();
            shortestTarget = _hero;


        } else {

            shortestX = _heroShadows[0]->getXPos();
            shortestY = _heroShadows[0]->getYPos();
            shortestTarget = _heroShadows[0];
        }

        for (HeroShadow *shadow : _heroShadows) {

            int shadowLength = qFabs(opponent->getXPos() - shadow->getXPos()) + qFabs(opponent->getYPos() - shadow->getYPos());
            int shortestLength = qFabs(opponent->getXPos() - shortestX) + qFabs(opponent->getYPos() - shortestY);
            if (shadowLength < shortestLength) {



                shortestX = shadow->getXPos();
                shortestY = shadow->getYPos();
                shortestTarget = shadow;
            }
        }


        if (opponent->getTargetHero() != nullptr) {


            if (opponent->getTargetHero()->getId() != shortestTarget->getId()) {
                opponent->setTargetHero(shortestTarget);
                opponent->directToTarget();
                qDebug() << "swiching";

            } else {

                if (heroId == opponent->getTargetHero()->getId()) {
                    opponent->directToTarget();
                }
            }
        } else {
            opponent->setTargetHero(shortestTarget);
            opponent->directToTarget();
        }
    }

}

void WorldView::shadowHeroMagic(int id, int withSword, int x, int y)
{

    for (HeroShadow *shadow : _heroShadows) {

        if (id == shadow->getId()) {

            shadow->shadowMagic(withSword, x, y);
        }
    }

}

void WorldView::shadowInjure(int id, int leftPercentage)
{

    for (int i = 0; i < _heroShadows.size();  i++) {

        if (id == _heroShadows[i]->getId()) {

            _heroShadows[i]->shadowInjure(leftPercentage);
            if (leftPercentage == 0) {
                _heroShadows.remove(i);
            }
            break;
        }
    }

    for (OpponentShadow *shadow : _opponentShadows) {

        if (id == shadow->getId()) {

            shadow->shadowInjure(leftPercentage);
        }
    }

}

void WorldView::shadowHealthIncresed(int id, int leftP)
{

    for (HeroShadow *shadow : _heroShadows) {

        if (shadow->getId() == id) {

            shadow->setHealthPercentage(leftP);
        }
    }

}

void WorldView::shadowMagicIncreased(int id, int leftP)
{
    for (HeroShadow *shadow : _heroShadows) {

        if (shadow->getId() == id) {

            shadow->setMagicPercentage(leftP);
        }
    }
}

void WorldView::shadowMagicUsed(int id, int leftP)
{
    for (HeroShadow *shadow : _heroShadows) {

        if (shadow->getId() == id) {

            shadow->setMagicPercentage(leftP);
        }
    }
}

void WorldView::roundOver(int round)
{

     //clean up all opponent shadows and opponents

    for (OpponentShadow *shadow : _opponentShadows) {

        this->scene()->removeItem(shadow);
        delete shadow;
    }
    _opponentShadows.clear();

    for (OpponentReal *opponent : _opponents) {

        if (opponent->isRunning()) {

            opponent->terminate();
        }
        this->scene()->removeItem(opponent);
        delete opponent;
    }
    _opponents.clear();



    emit roundOverConfirmed(round);
}

void WorldView::addGift(GiftModel gift)
{
    qDebug() << "adding a gift " << gift.id;

    Gift *myGift = new Gift(gift.id, gift.type, gift.x, gift.y);
    this->scene()->addItem(myGift);
    _gifts.push_back(myGift);
    myGift->setZValue(4);

}

void WorldView::removeGift(int giftId)
{
    for (int i = 0; i < _gifts.size(); i++) {

        if (_gifts[i]->getId() == giftId) {

            this->scene()->removeItem(_gifts[i]);
            _gifts.remove(i);
            break;
        }
    }
}

void WorldView::changeBackground(int color)
{
    for (WorldBase *base : _bases) {

        base->setColor(color);
    }
}



void WorldView::pauseGame()
{
    _isPaused = true;

    for (OpponentReal *opponent : _opponents) {

        opponent->setPause(true);
    }

    if (_hero != nullptr) {

        _hero->setPaused(true);
    }

}

void WorldView::resumeGame()
{
    _isPaused = false;

    for (OpponentReal *opponent : _opponents) {

        opponent->setPause(false);
    }
    if (_hero != nullptr) {

        _hero->setPaused(false);
    }
}





WorldBase *WorldView::getWorldBaseAtIndex(int index) const {

    return this->_bases[index];
}

void WorldView::keyPressEvent(QKeyEvent *event)
{
    if (_isPaused) return;

    if (event->key() == Qt::Key_1) {

        emit keyPressed(1);
    } else if (event->key() == Qt::Key_2) {

        emit keyPressed(2);
    } else if (event->key() == Qt::Key_3) {

        emit keyPressed(3);
    } else if (event->key() == Qt::Key_4) {

        emit keyPressed(4);
    }

    event->accept();
}

int WorldView::calculateHeroDir(int x, int y) {

    HeroReal *hero = this->_hero;

    int movingVectorX = x - hero->getXPos();
    int movingVectorY = y - hero->getYPos();

    int maxDir = 0;
    int maxProduct = 0;

    int unitVectorX1 = 0;
    int unitVectorY1 = -1000;
    int product1 = movingVectorX * unitVectorX1 + movingVectorY * unitVectorY1;
    if (product1 > maxProduct) {
        maxProduct = product1;
        maxDir = 1;
    }

    int unitVectorX2 = 707;
    int unitVectorY2 = -707;
    int product2 = movingVectorX * unitVectorX2 + movingVectorY * unitVectorY2;
    if (product2 > maxProduct) {
        maxProduct = product2;
        maxDir = 2;
    }

    int unitVectorX3 = 1000;
    int unitVectorY3 = 0;
    int product3 = movingVectorX * unitVectorX3 + movingVectorY * unitVectorY3;
    if (product3 > maxProduct) {
        maxProduct = product3;
        maxDir = 3;
    }

    int unitVectorX4 = 707;
    int unitVectorY4 = 707;
    int product4 = movingVectorX * unitVectorX4 + movingVectorY * unitVectorY4;
    if (product4 > maxProduct) {
        maxProduct = product4;
        maxDir = 4;
    }


    int unitVectorX5 = 0;
    int unitVectorY5 = 1000;
    int product5 = movingVectorX * unitVectorX5 + movingVectorY * unitVectorY5;
    if (product5 > maxProduct) {
        maxProduct = product5;
        maxDir = 5;
    }

    int unitVectorX6 = -707;
    int unitVectorY6 = 707;
    int product6 = movingVectorX * unitVectorX6 + movingVectorY * unitVectorY6;
    if (product6 > maxProduct) {
        maxProduct = product6;
        maxDir = 6;
    }

    int unitVectorX7 = -1000;
    int unitVectorY7 = 0;
    int product7 = movingVectorX * unitVectorX7 + movingVectorY * unitVectorY7;
    if (product7 > maxProduct) {
        maxProduct = product7;
        maxDir = 7;
    }

    int unitVectorX8 = -707;
    int unitVectorY8 = -707;
    int product8 = movingVectorX * unitVectorX8 + movingVectorY * unitVectorY8;
    if (product8 > maxProduct) {
        maxProduct = product8;
        maxDir = 8;
    }

    return maxDir;

}


