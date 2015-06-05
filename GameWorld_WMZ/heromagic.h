#ifndef HEROMAGIC_H
#define HEROMAGIC_H

#include <QThread>
#include <QColor>

class WorldView;
class Hero;

class HeroMagic : public QThread
{

    Q_OBJECT
public:
    HeroMagic(Hero *hero);
    void releaseAt(int withSword, int x, int y);
    void setWorld(WorldView *world);
    void setHero(Hero *hero);
signals:
    void highlightBeglin(QColor hightColot);
    void highlightEnd();

private:
    void run();
    int _x;
    int _y;
    WorldView *_world;

    Hero *_hero;

    int _range;
    int _injure;
    int _heal;
};

#endif // HEROMAGIC_H
