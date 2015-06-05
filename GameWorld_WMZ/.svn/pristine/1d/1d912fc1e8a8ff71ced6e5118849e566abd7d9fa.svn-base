#ifndef OPPONENT_H
#define OPPONENT_H


#include "worldpathfinder.h"
#include <QGraphicsPixmapItem>
#include "world.h"


class WorldView;
class StatusBar;


class Opponent : public QGraphicsPixmapItem, public Enemy
{

public:
    Opponent(int type, int id, const int &x, const int &y, int healthPercentage);
    ~Opponent();

    void setHealthPercentage(int percentage);
    int getId();



private:
    //private methods

    StatusBar *_healthBar;

    int _type;
    int _id;

};




#endif // OPPONENT_H
