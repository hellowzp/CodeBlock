#ifndef HERO_H
#define HERO_H

#include <QGraphicsPixmapItem>

#include "world.h"

#include <QObject>

#include "heromagic.h"
#include "statusbar.h"

class WorldView;
class QGraphicsTextItem;

class Hero : public QGraphicsPixmapItem, public Tile
{


public:
    explicit Hero(int role, int id, const QString &name, int x, int y, int healthPercentage, int magicPercentage);
    ~Hero();
    void setWorld(WorldView * world);
    HeroMagic & getMagic();
    void setHealthPercentage(int percentage);
    void setMagicPercentage(int percentage);
    int getRole() const;
    int getId();

protected:
    WorldView *_world;
    HeroMagic _magic{this};

private:
    //private methods


    StatusBar *_healthBar;
    StatusBar *_magicBar;



    int _id;
    int _role;

    QGraphicsTextItem *_nameLabel;

};

#endif // HERO_H
