#include "WorldView.h"
#include "MainWindow.h"

#include <QMouseEvent>
#include <QMessageBox>
#include <QApplication>

#include <algorithm>
#include <list>
#include <cassert>
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand

WorldView::WorldView(MainWindow* mainWindow):
    mainWindow(mainWindow)
{
    resetParas();

    scene = new QGraphicsScene;
    this->setScene(scene);
    scene->setSceneRect(0,0,this->world.getCols() * 34, this->world.getCols() * 34);
    this->setRenderHints(QPainter::Antialiasing);

    createWorld(true);

    this->setFixedSize(this->world.getCols() * 34, this->world.getCols() * 34);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void WorldView::resetParas() {
    animationTimer = -1;
    fightingTimer = -1;

    currentStep = 0;
    totalSteps = 0;
    nrLiveEnemies = NUM_ENEMY;

    heuristic = 1.0;
    animationFPS = 300;
    interactionMode = false;
}

void WorldView::createWorld(bool init) {
    if(init){ //only executed if called by the constructor
        std::vector<std::shared_ptr<Tile>> tiles = world.createWorld(QString(":/images/background_16.png"));
    //  std::random_shuffle( tiles.begin(), tiles.end() );
        std::srand ( unsigned ( std::time(0) ) );
        for (int j = 0; j < world.getRows(); j++) {
            for (int i = 0; i < world.getCols(); i++) {
                int r = std::rand() % tiles.size();
    //          int r = j * world.getCols() + i;
                float value = tiles[r]->getValue();
                if(value<0.3 || value>0.9) value = ( std::rand() % 7 )/10.0f + 0.3f;
    //            qDebug() << i << j << value;  //i->column->x
                TileItem* tile = new TileItem(i, j, value );
                addGraphicsItem(tile,4);
    //            base->setZValue(1);
    //            this->tiles.append(tile);
    //            scene->addItem(tile);
            }
        }
    }

    std::vector<std::shared_ptr<Tile>> targets = world.getHealthPacks(NUM_GIFT + NUM_ENEMY + 1);
    for(int i=0; i<NUM_GIFT; i++) {
//        Tile tile( (*targets[i]).getXPos(), (*targets[i]).getYPos(), (*targets[i]).getValue());
        StaticSpriteItem* gift = new StaticSpriteItem(*targets[i]);
        gift->setPixmap(QPixmap(":/images/apple-32.png"));
        addGraphicsItem(gift,3);
    }

    for(int i=NUM_GIFT; i<NUM_GIFT+NUM_ENEMY; i++) {
//        Enemy enemy = *enemies[i];
//        Tile tile(enemy.getXPos(), enemy.getYPos(), enemy.getValue());
        EnemySprite* esprite = new EnemySprite(*targets[i]);
        esprite->setPixmap(QPixmap(":/images/monster-32.png"));
        addGraphicsItem(esprite,2);
    }

    //sort enemies in ascending order by health, used in making strategy for the hero
    std::sort( enemies.begin(), enemies.end(),
               [](const EnemySprite* a, const EnemySprite* b) {
        return a->getValue() < b->getValue();
    });
    std::for_each( enemies.begin(), enemies.end(),
                   [](const EnemySprite* enemy) {
        qDebug() << "Enemy:" << enemy->getXPos() << enemy->getYPos() << enemy->getValue();
    });


    QPixmap pixmap(":/images/Super_Mario-32.png");
//    std::shared_ptr<Protagonist> protagonist = world.getProtagonist();
//    Tile tile( 0,0, getTileValue(0,0) );
    hero = new HeroSPrite(*targets[NUM_GIFT+NUM_ENEMY]);
    hero->setValue( hero->getValue() / 100.0f);
    hero->setPixmap(pixmap);
    addGraphicsItem(hero,1);

    QObject::connect(hero, SIGNAL(strategyChanged(QString,std::list<Tile>&)), this, SLOT(updateAnimationPath(QString,std::list<Tile>&)) );

//    this->setStyleSheet("QProgressBar { border: 1px solid red; }");
//    this->setStyleSheet("QProgressBar::chunk { background-color: #EE0000; width: 5px;}");

}

void WorldView::destroyWorld() {
    std::for_each(enemies.begin(),enemies.end(),
                  [this](EnemySprite* enemy) mutable {
        this->scene->removeItem(enemy);
        delete enemy;
    });
    enemies.clear();

    std::for_each(gifts.begin(),gifts.end(),
                  [this](StaticSpriteItem* gift) mutable {
        this->scene->removeItem(gift);
        delete gift;
    });
    gifts.clear();

    scene->removeItem(hero);
    delete(hero);

    if(!animationPath.empty()) unhighlightAnimatiomnPath();

    update();
}

WorldView::~WorldView()
{
    qDebug() << "WorldView::~WorldView()";
}

void WorldView::highlightAnimatiomnPath() {
    assert( !animationPath.empty());
    std::for_each( animationPath.begin(),animationPath.end(),
                   [this](const Tile& tile) {
         int x = tile.getXPos();
         int y = tile.getYPos();
         TileItem* tItem = this->getTileItem(x,y);
         float value = tItem->getValue();
         tItem->setBrush(QBrush(QColor(255, 255.0f * value, 0 )));
    });
}

void WorldView::unhighlightAnimatiomnPath() {
    assert( !animationPath.empty());
    std::for_each(animationPath.begin(),animationPath.end(),
                                    [this](const Tile& tile) {
             int x = tile.getXPos();
             int y = tile.getYPos();
             TileItem* tItem = this->getTileItem(x,y);
             float value = tItem->getValue();
             tItem->setBrush(QBrush(QColor(0, 255.0f * value, 0)) );
    });
}

void WorldView::updateGraphics() {
    qDebug() << "UpdatingGraphics:";
    if(animationPath.empty()) return;

    auto it = animationPath.rbegin();
    std::advance(it,currentStep);

    //current animation task not finished
    //should not dereference the end iterator
    //no error happens but may get random result
    if(it != animationPath.rend()) {
        int x = (*it).getXPos();
        int y = (*it).getYPos();
        qDebug() << "UpdatingGraphics:" << x << y;

        EnemySprite* enemy = getEnemy( x,y );
        if(enemy) {
            qDebug() << "begin to fight the enemy" << hero->getHealth() << enemy->getValue();

            currentStep = 0;
            unhighlightAnimatiomnPath();
            animationPath.clear();

            killTimer(animationTimer);
            animationTimer = 0;

            fightingEnemy = enemy;
            fightingTimer = startTimer(200);
            return;
        }

        StaticSpriteItem* gift = getGift(x,y);
        if(gift) removeGift(x,y);

        float energy = hero->energy - (*it).getValue();
        if(energy<=0) {
            hero->setEnergy(0.0f);
            loseGame();
        }else{
            hero->setEnergy(energy);
        }

        hero->updatePosition(x,y);

        currentStep++;
        totalSteps++;

    } else {

        currentStep = 0;
        unhighlightAnimatiomnPath();
        animationPath.clear();

        if(interactionMode){
            qDebug() << "animation timer" << animationTimer;
            killTimer(animationTimer);
            animationTimer = 0;
        }else{
            hero->makeStrategy();
        }
    }

    update();
}

void WorldView::mouseMoveEvent(QMouseEvent *ev){
    QApplication::sendEvent(scene,ev);
}

void WorldView::mousePressEvent(QMouseEvent* ev) {
    qDebug() << interactionMode;
    if( ev->button() != Qt::LeftButton || !interactionMode) return;

    if(animationTimer <= 0) {
        //the event will be received by the view,
        //mouse coordinates should be converted to scene coordinates
        //see http://qt-project.org/doc/qt-4.8/qgraphicsview.html#mapToScene
        //scene event handling: http://qt-project.org/doc/qt-4.8/qgraphicsscene.html#event-handling-and-propagation
        QPointF p = mapToScene(ev->pos());
        int x = p.x() / 34;
        int y = p.y() / 34;

        EnemySprite* enemy = getEnemy(x,y);
        if(enemy && enemy->getDefeated()) {
            //show error msg to choose a live enemy
            QMessageBox::information(this,"GameWorld",
                                     "This enemy has already defeated, please choose a live one.",
                                     QMessageBox::Ok);
            return;
        }

        float value = getTileValue(x,y);
        EnemySprite dummySprite(Tile(x,y,value));
        //type cast will create a new temporary object
        animationPath = static_cast<MoveableSpriteItem*>(hero)->findShortestPath(dummySprite);

        //change the accumulated cost to single-step cost
        float cost = 0.0f;
        std::for_each( animationPath.rbegin(), animationPath.rend(),
                       [cost](Tile& tile) mutable {
            float t_cost = tile.getValue();
            tile.setValue( 5 * t_cost - 5 * cost);
            cost = t_cost;
        });
        highlightAnimatiomnPath();

        if(animationTimer <= 0) animationTimer = startTimer(animationFPS);  //FPS

    }
}

void WorldView::timerEvent(QTimerEvent* ev) {
    if( ev->timerId() == animationTimer ) updateGraphics();
    if( ev->timerId() == fightingTimer ) defeatEnemy();
}

//private wrapper function to call when add items
//to make sure the worldView is simultaneously set in the item class
void WorldView::addGraphicsItem(QGraphicsItem* item, int type) {
    assert(scene != nullptr);
    scene->addItem(item);

    if(type==4) { //tileItem
        TileItem* tile = static_cast<TileItem*>(item);
        tiles.append(tile);
        return;
    }

    if(type==1) {  //hero
        hero->setWorldView(this);
    }
    if(type==2) {  //monster
        EnemySprite* enemy = static_cast<EnemySprite*>(item);
        enemies.append(enemy);
        enemy->setWorldView(this);
    }
    if(type==3) { //gift
        StaticSpriteItem* gift = static_cast<StaticSpriteItem*>(item);
        gifts.append(gift);
        gift->setWorldView(this);
    }

    scene->addWidget(static_cast<SpriteItem*>(item)->getPropertyView());
}

EnemySprite* WorldView::getEnemy(int x, int y) const {
    qDebug() << "getEnemy()" << x << y;
    assert(x>=0 && y>=0 && x<MAP_ROWS && y<MAP_COLS);
    auto it = std::find_if( enemies.begin(), enemies.end(),
                   [x,y](const EnemySprite* enemy) {
        return enemy->getXPos() == x && enemy->getYPos() == y;
    });
    if( it != enemies.end() ) return *it;
    return nullptr;
}

StaticSpriteItem* WorldView::getGift(int x, int y) const {
    assert(x>=0 && y>=0 && x<MAP_ROWS && y<MAP_COLS);
    auto it = std::find_if( gifts.begin(), gifts.end(),
                   [x,y](const StaticSpriteItem* gift) {
        return gift->getXPos() == x && gift->getYPos() == y;
    });
    if( it != gifts.end() ) return *it;
    return nullptr;
}

SpriteItem* WorldView::getSpriteItem(int x, int y) const {
    SpriteItem* sprite = getEnemy(x,y);
    if(sprite) return sprite;
    sprite = getGift(x,y);
    if(sprite) return sprite;
    if(hero->getXPos()==x && hero->getYPos()==y) return hero;
    return nullptr;
}

TileItem* WorldView::getTileItem(int x, int y) const {
    assert(x>=0 && y>=0 && x<MAP_ROWS && y<MAP_COLS);
    return tiles.at( x + y * MAP_COLS );
}

float WorldView::getTileValue(int x, int y) const {
    return this->getTileItem(x,y)->getValue();
}

int WorldView::getTileDistance(const Tile& a, const Tile& b) const {
    int dx = a.getXPos() - b.getXPos();
    int dy = a.getYPos() - b.getYPos();
    return (dx<0?-dx:dx) + (dy<0?-dy:dy);
}

void WorldView::defeatEnemy() {
    float enemyHealth = fightingEnemy->getValue();
    float heroHealth  = hero->getHealth();

    if(enemyHealth<=5 || heroHealth<=5) {
        killTimer(fightingTimer);
        fightingTimer = 0;
        if(enemyHealth<=heroHealth){
            hero->setHealth(heroHealth-enemyHealth);
            enemyDefeated();
        }else{
            fightingEnemy->setValue(enemyHealth-heroHealth);
            fightingEnemy->updatePropertyView();
            hero->setHealth(0.0f);
            loseGame();
        }
    }
    if(enemyHealth>5  && heroHealth>5 ) {
        fightingEnemy->setValue(enemyHealth-5);
        fightingEnemy->updatePropertyView();
        hero->setHealth(heroHealth-5);
    }
}

void WorldView::enemyDefeated() {
    QPixmap pixmap(":/images/monster-32_defeated.png");
    fightingEnemy->setValue(0.0f);
    fightingEnemy->setPixmap(pixmap);
    fightingEnemy->hidePropertyView();
    fightingEnemy->setDefeated(true);
    qDebug() << "\nEnemy defeated";

    hero->setEnergy(100.0f);

    nrLiveEnemies--;
    if(nrLiveEnemies==0){
        winGame();
    }else if(!interactionMode){
        hero->makeStrategy();
    }
}

void WorldView::removeGift(int x, int y) {
    auto it = std::find_if( gifts.begin(), gifts.end(),
                            [x,y](const StaticSpriteItem* gift) {
        return gift->getXPos() == x && gift->getYPos() == y;
    });

    assert(it != gifts.end());

    float health = hero->health + (*it)->getValue();
    if( health > 100.0f ) health = 100.0F;
    hero->setHealth( health );
    scene->removeItem(*it);
//    (*it)->hidePropertyView();

    delete(*it);
    gifts.erase(it);
}

QVector<std::shared_ptr<Vertex>> WorldView::getNeighboringVertexs(const Vertex& pVertex) const {
    int x = pVertex.getXPos();
    int y = pVertex.getYPos();
    assert(x>=0 && y>=0 && x<MAP_ROWS && y<MAP_COLS);

    QVector<std::shared_ptr<Vertex>> vertexs;
//    qDebug() << vertexs.capacity();  //0
    vertexs.reserve(4);
    float value = pVertex.getValue();
    qDebug() << "WorldView::getNeighboringVertexs" << x << y << value;
    float nValue = 0.0f, gCost = 0.0f, block_value = 1.0f;
    if(y-1>=0) {
        nValue = getTileValue(x,y-1);
        if(nValue<block_value) {  //not a block
            gCost = value - nValue;
            gCost = ( gCost>0 ? gCost : -gCost );
            auto vertex = std::make_shared<Vertex>(x,y-1,nValue,gCost,&pVertex);
            vertexs.append(vertex);
        }
    }
    if(x+1<MAP_COLS) {
        nValue = getTileValue(x+1,y);
        if(nValue<block_value) {  //not a block
            gCost = value - nValue;
            gCost = ( gCost>0 ? gCost : -gCost );
            auto vertex = std::make_shared<Vertex>(x+1,y,nValue,gCost,&pVertex);
            vertexs.append(vertex);
        }
    }
    if(y+1<MAP_ROWS) {
        nValue = getTileValue(x,y+1);
        if(nValue<block_value) {  //not a block
            gCost = value - nValue;
            gCost = ( gCost>0 ? gCost : -gCost );
            auto vertex = std::make_shared<Vertex>(x,y+1,nValue,gCost,&pVertex);
            vertexs.append(vertex);
        }
    }
    if(x-1>=0) {
        nValue = getTileValue(x-1,y);
        if(nValue<block_value) {  //not a block
            gCost = value - nValue;
            gCost = ( gCost>0 ? gCost : -gCost );
            auto vertex = std::make_shared<Vertex>(x-1,y,nValue,gCost,&pVertex);
            vertexs.append(vertex);
        }
    }
    return vertexs;
}

void WorldView::updateAnimationPath(QString strategy, std::list<Tile>& path) {
    qDebug() << "updating animationPath";
    animationPath = path;
    //change the accumulated cost to single-step cost and multiply by 100
    float cost = 0.0f;
    std::for_each( animationPath.rbegin(), animationPath.rend(),
                   [cost](Tile& tile) mutable {
        float t_cost = tile.getValue();
        tile.setValue( COST_FACTOR * t_cost - COST_FACTOR * cost);
        cost = t_cost;
    });
    highlightAnimatiomnPath();

    std::for_each( animationPath.rbegin(), animationPath.rend(),
                   [](Tile& tile) {
        qDebug() << tile.getXPos() << tile.getYPos() << tile.getValue();
    });

    if(animationTimer <= 0) animationTimer = startTimer(animationFPS);
}

void WorldView::changeFPS(int value) {
    animationFPS = value * 100;
    if(animationTimer>0 ) {
        killTimer(animationTimer);
        animationTimer = startTimer(animationFPS);
    }
}

void WorldView::start() {
    if(animationTimer == -1) {  //the first start
        hero->makeStrategy();
    }else if(animationTimer == -2){
        animationTimer = startTimer(animationFPS);
    }

    if(fightingTimer == -2){
        fightingTimer = startTimer(200);
    }
}

void WorldView::pause() {
    if(animationTimer>0){
        killTimer(animationTimer);
        animationTimer = -2;
    }
    if(fightingTimer>0){
        killTimer(fightingTimer);
        fightingTimer = -2;
    }
}

void WorldView::restart(){
    resetParas();
    createWorld(false);

    //after deleting the hero, all the signal-slot connections
    //related to the hero will also be releaed automatically
    //so reset those connections are needed
    mainWindow->resetHeroConnections();

    emit gameRestarted();
}

void WorldView::stop(){
    if(animationTimer>0) {
        killTimer(animationTimer);
        animationTimer = -1;
    }
    if(fightingTimer>0) {
        killTimer(fightingTimer);
        fightingTimer = -1;
    }

    destroyWorld();
}

void WorldView::winGame() {
    if(animationTimer>0) killTimer(animationTimer);
    QMessageBox::StandardButton result = QMessageBox::question(this,"GameWorld","Congratulations, You win the game!\nDo you want to restart?",
                                         QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::Yes){
        destroyWorld();
        restart();
    }else{
        mainWindow->close();
    }
}

void WorldView::loseGame() {
    if(animationTimer>0) killTimer(animationTimer);
    if(fightingTimer>0)  killTimer(fightingTimer);
    QMessageBox::StandardButton result = QMessageBox::question(this,"GameWorld","OOh, You lose the game.\nDo you want to restart?",
                                         QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::Yes){
        destroyWorld();
        restart();
    }else{
        mainWindow->close();
    }
}
