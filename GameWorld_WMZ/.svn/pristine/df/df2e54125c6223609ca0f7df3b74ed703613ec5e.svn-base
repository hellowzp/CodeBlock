#include "world.h"
#include <QImage>
#include <QRgb>
#include <QDateTime>


World::World()
  {
  }

std::vector<Tile *> World::createWorld(QString filename)
  {
  //read image in, every pixel greyvalue is value of tile
  QImage world(filename);
  if (world.isNull())
    throw "Unable to read image file " + filename;
  rows = world.height();
  cols = world.width();
  std::vector<Tile *> tiles;
  tiles.reserve(rows*cols);
  for (int row = 0; row < rows; row++)
    {
    for (int col = 0; col < cols; col++)
      {
      tiles.push_back(new Tile(col, row, qGray(world.pixel(col,row))/255.0f));
      }
    }
  return tiles;
  }

std::vector<Enemy *> World::getEnemies(int nrOfEnemies)
  {
  qsrand(QDateTime::currentMSecsSinceEpoch());
  std::vector<Enemy *> enemies;
//  enemies.reserve(nrOfEnemies);
  while (enemies.size() < (uint)nrOfEnemies)
    {
    int xPos = qrand() % cols;
    int yPos = qrand() % rows;
    Enemy * eptr = new Enemy(xPos, yPos, qrand()%100);
    if (xPos != 0 || yPos != 0)
      {
      if (std::find_if(enemies.begin(), enemies.end(), [eptr](const Enemy * p)-> bool
                                                       {return *p == *eptr;}) == enemies.end())
        enemies.push_back(eptr);
      }
    }
  return enemies;
  }

std::vector<float> World::getHealthPacks(int nrOfPacks)
  {
  qsrand(QDateTime::currentMSecsSinceEpoch());
  std::vector<float> healthPacks;
  healthPacks.reserve(nrOfPacks);
  for (int number = 0; number < nrOfPacks; number++)
    healthPacks.push_back(qrand()%100);
  return healthPacks;
  }


Tile::Tile(int xPosition, int yPosition, float tileWeight):
  xPos(xPosition), yPos(yPosition), value(tileWeight)
  {
  }



Enemy::Enemy(int xPosition, int yPosition, float strength):
  Tile(xPosition, yPosition, strength)
  {
  }
