#ifndef MOVEABLESPRITEITEM_H
#define MOVEABLESPRITEITEM_H

#include <QVector>
#include <list>
#include <memory>

#include "SpriteItem.h"

class WorldView; //should include it in the cpp file
struct Vertex;

class MoveableSpriteItem : public SpriteItem
{
    Q_OBJECT

public:
    MoveableSpriteItem(const Tile& tile);
    virtual ~MoveableSpriteItem();
    MoveableSpriteItem(const MoveableSpriteItem& other);

    std::list<Tile> getPath() const { return path; }

    void moveToDestination(SpriteItem &dest);
    void updatePosition(int x, int y);

    std::list<Tile> findShortestPath(const SpriteItem& dest) const;
    float           getLowestCost(const SpriteItem& dest) const;

protected:
    std::list<Tile> path;
    std::list<Tile> reachableScope;

private:
    //see Item 35 NVI: non-virtual interface
    std::shared_ptr<Vertex> parseToVertex() const;

    friend struct TileComparator;

signals:
    void strategyChanged(QString strategy, std::list<Tile>& path);

};

//http://stackoverflow.com/questions/15646451/stl-priority-queue-and-overloading-with-pointers
struct TileComparator {

    TileComparator(const MoveableSpriteItem* h, const SpriteItem* item):host(h), dest(item) {}

    // a functor: override () operator to be callable
    // inline to improve efficiency: effective STL item 46
    inline bool operator() (const std::shared_ptr<Vertex>& a, const std::shared_ptr<Vertex>& b) const;

    const MoveableSpriteItem* host = nullptr;
    const SpriteItem* dest = nullptr;
};

#endif // MOVEABLESPRITEITEM_H
