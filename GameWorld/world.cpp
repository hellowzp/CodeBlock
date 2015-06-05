#include "world.h"
#include <QImage>
#include <QRgb>
#include <QDateTime>


World::World()
  {
  }

std::vector<std::shared_ptr<Tile>> World::createWorld(QString filename)
  {
  //read image in, every pixel greyvalue is value of tile
  QImage world(filename);
  if (world.isNull())
    throw "Unable to read image file " + filename;
  rows = world.height();
  cols = world.width();
  std::vector<std::shared_ptr<Tile>> tiles;
  tiles.reserve(rows*cols);
  for (int row = 0; row < rows; row++)
    {
    for (int col = 0; col < cols; col++)
      {
      tiles.push_back(std::make_shared<Tile>(col, row, qGray(world.pixel(col,row))/255.0F));
      }
    }
  return tiles;
  }

std::vector<std::shared_ptr<Enemy>> World::getEnemies(unsigned int nrOfEnemies)
  {
  qsrand(QDateTime::currentMSecsSinceEpoch());
  std::vector<std::shared_ptr<Enemy>> enemies;
  enemies.reserve(nrOfEnemies);
  while (enemies.size() < nrOfEnemies)
    {
    int xPos = qrand() % cols;
    int yPos = qrand() % rows;
    auto eptr = std::make_shared<Enemy>(xPos, yPos, qrand()%100);
    if (xPos != 0 || yPos != 0)
      {
      if (std::find_if(enemies.begin(), enemies.end(),
                        [eptr](std::shared_ptr<const Enemy> p)-> bool {
                                return *p == *eptr;
                        }) == enemies.end() )
        enemies.push_back(eptr);
      }
    }
  return enemies;
  }

std::vector<std::shared_ptr<Tile> > World::getHealthPacks(unsigned int nrOfPacks)
  {
  std::vector<std::shared_ptr<Tile>> healthPacks;
  healthPacks.reserve(nrOfPacks);
  while (healthPacks.size() < nrOfPacks)
    {
    int xPos = qrand() % cols;
    int yPos = qrand() % rows;
    auto hptr = std::make_shared<Tile>(xPos, yPos, qrand()%100);
    if (xPos != 0 || yPos != 0)
      {
      if (std::find_if(healthPacks.begin(), healthPacks.end(), [hptr](std::shared_ptr<const Tile> p)-> bool
                                                       {return *p == *hptr;}) == healthPacks.end())
        healthPacks.push_back(hptr);
      }
    }
  return healthPacks;
  }

std::shared_ptr<Protagonist> World::getProtagonist()
  {
  return std::make_shared<Protagonist>();
  }


Tile::Tile(int xPosition, int yPosition, float tileWeight):
  xPos(xPosition), yPos(yPosition), value(tileWeight)
  {
  }



Enemy::Enemy(int xPosition, int yPosition, float strength):
  Tile(xPosition, yPosition, strength), defeated{false}
  {
    }
bool Enemy::getDefeated() const
    {
    return defeated;
    }

void Enemy::setDefeated(bool value)
    {
    defeated = value;
    }


Protagonist::Protagonist(): Tile(0, 0, 0.0f), health{100.0f}, energy{100.0f}
    {
    }
float Protagonist::getHealth() const
    {
    return health;
    }

void Protagonist::setHealth(float value)
    {
    health = value;
    }
float Protagonist::getEnergy() const
    {
    return energy;
    }

void Protagonist::setEnergy(float value)
    {
    energy = value;
    }


