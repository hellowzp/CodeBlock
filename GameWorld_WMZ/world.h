#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <QString>
class Tile
  {
  public:
    Tile(int xPosition, int yPosition, float tileWeight);
    float getValue() const {return value;}
    void setValue(float newValue) {value = newValue;}
    int getXPos() const {return xPos;}
    int getYPos() const {return yPos;}
    void setXPos(int newPos) {xPos = newPos;}
    void setYPos(int newPos) {yPos = newPos;}

  private:
    int xPos;
    int yPos;
    float value;
  };

class Enemy : public Tile
  {
  public:
    Enemy(int xPosition, int yPosition, float strength);
    bool operator== (const Enemy & other) const
      {return (getXPos() == other.getXPos()) && (getYPos() == other.getYPos());}
  };

class World
  {
  public:
    World();
    std::vector<Tile *> createWorld(QString filename);
    std::vector<Enemy *> getEnemies(int nrOfEnemies);
    std::vector<float> getHealthPacks(int nrOfPacks);
    int getRows() const {return rows;}
    int getCols() const {return cols;}
  private:
    int rows, cols;
  };


#endif // WORLD_H
