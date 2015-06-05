#include "StaticSpriteItem.h"
#include <QDebug>

#include <ctime>
#include <cstdlib>

StaticSpriteItem::StaticSpriteItem(const Tile& tile):
    SpriteItem(tile)
{
    float v = getValue();
    if( v<10 || v>90 ) {
        std::srand ( unsigned ( std::time(0)) );
        setValue( std::rand() % 64 + 20 );
    }
    qDebug() << "StaticSpriteItem::StaticSpriteItem" << getValue() << getID();
}

StaticSpriteItem::~StaticSpriteItem()
{
    qDebug() << "StaticSpriteItem::~StaticSpriteItem" << getID();
}

//StaticSpriteItem::StaticSpriteItem(const StaticSprite& other)
//{
//    //copy ctor
//}

//StaticSpriteItem& StaticSpriteItem::operator=(const StaticSprite& rhs)
//{
//    if (this == &rhs) return *this; // handle self assignment
//    //assignment operator
//    return *this;
//}


