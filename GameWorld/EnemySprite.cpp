#include "EnemySprite.h"
#include "TileItem.h"

#include <QDebug>
#include <ctime>
#include <cstdlib>

EnemySprite::EnemySprite(const Tile &tile):
    MoveableSpriteItem(tile), defeated{false}
{
    float v = getValue();
    if( v<10 || v>90 ) {
        std::srand ( unsigned ( std::time(0)) );
        setValue( std::rand() % 64 + 20 );
    }
    qDebug() << "EnemySprite::EnemySprite" << getValue() << getID();
}

EnemySprite::~EnemySprite()
{
    qDebug() << "EnemySprite::~EnemySprite" << getID();
}

