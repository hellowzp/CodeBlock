#ifndef HEROREAL_H
#define HEROREAL_H

#include <QThread>
#include "worldviewanalyzer.h"
#include "hero.h"

class HeroReal: public QThread, public Hero
{
    Q_OBJECT

public:
    HeroReal(int role, int id, const QString &name, int x, int y, int healthPercentage, int magicPercentage);
    ~HeroReal();

    void faceToDirection(int d);
    void startMoving();
    void stopMoving();

    int calculateHealthPercentage();
    int calculateMagicPercentage();
    void setPaused(bool paused);
    void setProgressBars();
    void healHero(int value);

    void releaseMagicAt(int x, int y);
    bool isAlive();


signals:
    void needToMove();
    void heroMoved(int id, int x, int y);
    void heroMovingStoped(int id);
    void needToRepathOpponents(int heroId);
    void magicReleased(int id, int withSword, int x, int y);
    void heroInjured(int id, int leftPercentage);
    void giftPicked(int heroId, int giftId, int giftType);
    void heroHealthIncreased(int id, int leftP);
    void heroMagicIncreased(int id, int leftP);
    void heroMagicUsed(int id, int leftP);
    void updateProgressBars(int type, int value, int total);



private slots:
    void moveOneStep();
    void injureHero(int value);
    void equipmentChanged(bool sword, bool armor, bool boot);
    void drinkEnergy(int type);
    void magicAutoIncrease();

private:
    void run();

    int _healthValue;
    int _totalHealth;
    int _magicValue;
    int _totalMagic;
    int _speed = 5;

    bool _isPaused;

    QTimer *_magicAutoIncreaseTimer;

    bool _sword;
    bool _armor;
    bool _boot;

    int _facingDir;
    int _movingCounter;

    bool _isAlive;

};

#endif // HEROREAL_H
