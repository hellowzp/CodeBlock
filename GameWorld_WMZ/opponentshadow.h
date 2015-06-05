#ifndef OPPONENTSHADOW_H
#define OPPONENTSHADOW_H

#include "opponent.h"

class OpponentShadow :public Opponent
{

public:
    OpponentShadow(int type, int id, const int &x, const int &y, int healthPercentage);
    ~OpponentShadow();
    void moveToPoint(int x, int y);
    void shadowInjure(int leftPercentage);

private:



};

#endif // OPPONENTSHADOW_H
