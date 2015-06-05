#ifndef ENEMYSPRITE_H
#define ENEMYSPRITE_H

#include "MoveableSpriteItem.h"

class EnemySprite : public MoveableSpriteItem
{

private:
    bool defeated;

public:
    explicit EnemySprite(const Tile &tile);
    ~EnemySprite();

    bool getDefeated() const { return defeated; }
    void setDefeated(bool value) { defeated = value; }

};

#endif // ENEMYSPRITE_H
