#ifndef GIFT_H
#define GIFT_H

#include <QGraphicsPixmapItem>
#include "world.h"

class Gift : public QGraphicsPixmapItem, public Tile
{
public:
    Gift(int id, int type, int x, int y);

    int getId() const;
    int getType() const;


public:
    const int _id;
    const int _type;

};

#endif // GIFT_H
