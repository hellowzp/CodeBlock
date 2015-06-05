#ifndef STATICSPRITE_H
#define STATICSPRITE_H

#include "world.h"
#include "SpriteItem.h"
#include <QGraphicsPixmapItem>
#include <QString>


class StaticSpriteItem : public SpriteItem
{
    Q_OBJECT

public:
    StaticSpriteItem(const Tile& tile);
    virtual ~StaticSpriteItem();
//    StaticSpriteItem(const StaticSpriteItem& other);
//    StaticSpriteItem& operator=(const StaticSpriteItem& other);


protected:
private:
};

#endif // STATICSPRITE_H
