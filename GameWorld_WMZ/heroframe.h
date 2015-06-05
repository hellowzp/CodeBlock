#ifndef HEROFRAME_H
#define HEROFRAME_H

#include <QFrame>
#include "gamehoster.h"

class HeroFrame : public QFrame
{
    Q_OBJECT
public:
    explicit HeroFrame(HeroModel &hero);
    ~HeroFrame();


signals:

public slots:


private:

    HeroModel _hero;
};

#endif // HEROFRAME_H
