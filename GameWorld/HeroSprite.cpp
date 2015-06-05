#include "HeroSprite.h"
#include "WorldView.h"

#include <cassert>

HeroSPrite::HeroSPrite(const Tile& tile):
    MoveableSpriteItem(tile), health{100.0f}, energy{100.0f}
{
    qDebug() << "HeroSPrite::HeroSPrite" << getID();
}

HeroSPrite::~HeroSPrite()
{
    qDebug() << "HeroSPrite::~HeroSPrite" << getID();
}

void HeroSPrite::makeStrategy() {
    qDebug() << "make strategy...";

    WorldView* worldView = getWorldView();
    auto enemies = worldView->getEnemies();

    //enemies are already sorted in ascending order by health at creation
    //now first find the weaker enemies and then choose the nearest to fight against
    float heroHealth = this->getHealth();
    auto it = std::find_if(enemies.begin(), enemies.end(),
                           [heroHealth](const EnemySprite* enemy) {
        return enemy->getValue() > heroHealth;
    });

    int min_ds = 100, ds = 100; //just big enough
    int x = -1, y = -1;

    //capture the outside variable by reference in order to change it externally
    //cauze otherwise lambda just make a copy of those variables by value
    //so x, y is possible to be changed by ds will remain the same
    std::for_each( enemies.begin(), it,
                   [worldView,this,ds,min_ds,&x,&y](const EnemySprite* enemy) mutable {
        qDebug() << "this->getLowestCost(*enemy)"  << this->getLowestCost(*enemy) << this->energy;
        if( !enemy->getDefeated() &&
            this->getLowestCost(*enemy) < this->energy ){
                ds = worldView->getTileDistance( *this, *enemy);
                if( ds < min_ds ) {
                    x = enemy->getXPos();
                    y = enemy->getYPos();
                    min_ds = ds;
                }
        }
    });

    if(x>=0 && y>=0) {
        EnemySprite* enemy = worldView->getEnemy(x,y);
        assert(enemy);
        if(!enemy->getDefeated()) {
            moveToDestination(*enemy);
            return;
        }
    }

    //no weaker live enemy found,
    //if there is still enemies but no health pack left,
    //just fight against the strongest live enermy and die..
    //otherwise move to the nearest pack
    auto gifts = worldView->getGifts();
    if(gifts.empty()) {
        auto it = std::find_if(enemies.begin(), enemies.end(),
                               [heroHealth](const EnemySprite* enemy) {
            return !enemy->getDefeated();
        });
        moveToDestination(**it);
    } else {

        x = 0, y = 0;
        std::for_each( gifts.begin(), gifts.end(),
                       [worldView,this,ds,min_ds,&x,&y](const StaticSpriteItem* gift) mutable {
            ds = worldView->getTileDistance( *this, *gift);
            if( ds < min_ds ) {
                x = gift->getXPos();
                y = gift->getYPos();
                min_ds = ds;
            }
        });

        StaticSpriteItem* gift = worldView->getGift(x,y);
        assert(gift);
        moveToDestination(*gift);
    }
}

