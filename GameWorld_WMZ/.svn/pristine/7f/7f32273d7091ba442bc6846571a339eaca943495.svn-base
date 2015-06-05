#ifndef WORLDVIEW_H
#define WORLDVIEW_H

#include <QGraphicsView>
#include <QWidget>
#include <QVector>
#include <QMutex>
#include "gamehoster.h"
#include "heroshadow.h"
#include "heroreal.h"
#include "assetsframe.h"

class WorldBase;
class WorldViewAnalyzer;
class Opponent;
class Hero;
class OpponentShadow;
class OpponentReal;
class QMessageBox;
class Gift;
class MainWindow;


class WorldView : public QGraphicsView
{

    Q_OBJECT


public:
    WorldView();
    explicit WorldView(const QString & url);
    ~WorldView();



    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void setAssetsFrame(AssetsFrame *frame);
    void setMainWindow(MainWindow *mainWindow);


signals:
    void heroCreated(HeroModel hero);
    void OpponentCreated(OpponentModel opponent);
    void roundOverConfirmed(int round);
    void keyPressed(int key);

public slots:
    void drawMagic();
    void createHero(HeroModel hero);
    void addHeroShadow(HeroModel hero);
    void addOpponentShadow(OpponentModel opponent);
    void moveShadow(int id, int x, int y);
    void createOpponent(int type, int id, int x, int y);
    void repathOpponents(int heroId);
    void shadowHeroMagic(int id, int withSword, int x, int y);
    void shadowInjure(int id, int leftPercentage);
    void shadowHealthIncresed(int id, int leftP);
    void shadowMagicIncreased(int id, int leftP);
    void shadowMagicUsed(int id, int leftP);
    void roundOver(int round);
    void addGift(GiftModel gift);
    void removeGift(int giftId);

    void changeBackground(int color);
    void pauseGame();
    void resumeGame();


    WorldBase *getWorldBaseAtIndex(int index) const;


private:

    QVector<OpponentReal *> _opponents;
    HeroReal *_hero;
    QVector<HeroShadow *> _heroShadows;
    QVector<OpponentShadow *> _opponentShadows;
    QVector<Gift *> _gifts;

    int _worldWidth;
    int _worldHeight;
    World *_world;

    void keyPressEvent(QKeyEvent *event);

    std::vector<WorldBase *> _bases;

    int calculateHeroDir(int x, int y);

    friend class WorldViewAnalyzer;
    friend class HeroReal;
    friend class Opponent;
    friend class HeroMagic;
    friend class GameBuider;
    friend class OpponentReal;

    AssetsFrame *_assetsFrame;
    MainWindow *_mainWindow;

    bool _isPaused;

};

#endif // WORLDVIEW_H
