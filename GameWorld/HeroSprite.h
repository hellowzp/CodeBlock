#ifndef HEROSPRITE_H
#define HEROSPRITE_H

#include "MoveableSpriteItem.h"

#include <deque>

class EnemySprite;

class HeroSPrite : public MoveableSpriteItem
{
    Q_OBJECT

private:
    float health;
    float energy;

    std::deque<SpriteItem*> targetQueue;

    friend class WorldView;

public:
    explicit HeroSPrite(const Tile& tile = Tile(0, 0, 0.0f) );
    ~HeroSPrite();

    void setXPos(int newPos) { xPos = newPos; emit posChanged(xPos, yPos);}
    void setYPos(int newPos) { yPos = newPos; emit posChanged(xPos, yPos);}

    float getHealth() const { return health; }
    void  setHealth(float value) {
        health = value;
        emit healthChanged( health<1.0f && health>0.0f ? 1 : health);
    }

    float getEnergy() const { return energy; }
    void  setEnergy(float value) {
        energy = value;
        emit energyChanged( energy<1.0f && energy>0.0f ? 1 : energy);
    }

    void pickupGift(int x, int y);
    void makeStrategy();


signals:
    void posChanged(int x, int y);
    void healthChanged(int health);
    void energyChanged(int energy);

};

#endif // HEROSPRITE_H
