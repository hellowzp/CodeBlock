#ifndef WORLDVIEW_H
#define WORLDVIEW_H

#include <QGraphicsView>
#include <QWidget>
#include <QVector>
#include <QDebug>

#include "HeroSprite.h"
#include "EnemySprite.h"
#include "StaticSpriteItem.h"
#include "TileItem.h"
#include "world.h"

class MainWindow;
struct Vertex;   //only for type declaration

class WorldView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit WorldView(MainWindow *mainWindow);
    ~WorldView();


    void  setheuristic(float h) { heuristic = h; }
    float getheuristic() const { return heuristic; }


    void highlightAnimatiomnPath();
    void unhighlightAnimatiomnPath();

    MainWindow* getMainWindow() const { return mainWindow; }
    HeroSPrite* getHero() const { return hero; }

    //for a const funtion that returns a reference to a data member
    //the returned reference must also be const
    //otherwise you are attempting to convert from const member to non-const member reference
    //see http://stackoverflow.com/questions/4957025/const-methods-that-return-references
    const QVector<EnemySprite*>& getEnemies() const { return enemies; }
    const QVector<StaticSpriteItem*>& getGifts() const { return gifts; }

    EnemySprite* getEnemy(int x, int y) const;
    StaticSpriteItem* getGift(int x, int y) const;
    TileItem*    getTileItem(int x, int y) const;
    SpriteItem*  getSpriteItem(int x, int y) const;
    float        getTileValue(int x, int y) const;
    int          getTileDistance(const Tile& a, const Tile& b) const;

    QVector<std::shared_ptr<Vertex>> getNeighboringVertexs(const Vertex& vertex) const;
    void updateGraphics();

    void setInteraction(bool interact){interactionMode = interact;}
    void setAnimationFPS(int fps) { animationFPS  = fps; }

    void removeGift(int x, int y);
    void defeatEnemy();
    void enemyDefeated();

    void start();
    void pause();
    void stop();
    void restart();

    void winGame();
    void loseGame();

    static const int MAP_ROWS = 16;
    static const int MAP_COLS = 16;
    static const int NUM_ENEMY = 8;
    static const int NUM_GIFT = 8;

    static const int TILE_SIZE = 34;
    static const int COST_FACTOR = 20;

protected:
//    void paintEvent( QPaintEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void timerEvent( QTimerEvent *ev);
//    void wheelEvent( QWheelEvent *ev);


private:

    //init all the tiles, roles and gifts
    void createWorld(bool init);
    void destroyWorld();
    void resetParas();
    void addGraphicsItem(QGraphicsItem *item, int type);

    MainWindow* mainWindow;
    QGraphicsScene* scene;
    HeroSPrite* hero;
    QVector<EnemySprite*> enemies;
    QVector<StaticSpriteItem*> gifts;
    QVector<TileItem*> tiles;
    World world;
    EnemySprite* fightingEnemy;

    int animationTimer;
    int fightingTimer;

    int currentStep;
    int totalSteps;
    int nrLiveEnemies;

    float heuristic;
    int animationFPS;
    bool interactionMode;

    //points from the target to the source(exclusive)
    //update it each time the hero make a new strategy
    std::list<Tile> animationPath;

//    friend class MoveableSpriteItem;
//    friend class HeroSprite;
//    friend class EnemySprite;
//    friend class StaticSpriteItem;
//    friend class DisplayView;
    friend class MainWindow;

signals:
    void gameRestarted();

public slots:
    void updateAnimationPath(QString strategy, std::list<Tile>& path);
    void changeFPS(int value);

    void changeheuristic(int value){
        heuristic = value / 10.0f;
    }

    void changeInteraction(bool interact){
        interactionMode = interact;
        //no matter it's going from animation mode to interaction mode
        //or toggling between interaction checkbox
        //game should first be paused to kill any timer
        pause();
        animationTimer = -1;
        if(!animationPath.empty()) unhighlightAnimatiomnPath();
    }

    void togglePause(bool paused) {
        paused?pause():start();
    }

    void toggleStop(bool stopped) {
        stopped?stop():restart();
    }

};

struct Vertex : public Tile {
    float GCost;
    Vertex* parent;

    Vertex(int x, int y, float v, float g_cost=0.0f, const Vertex* p = nullptr ):
        Tile(x,y,v), GCost(g_cost), parent(const_cast<Vertex*>(p))
    {

    }
};

#endif // WORLDVIEW_H
