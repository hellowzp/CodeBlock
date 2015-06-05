#ifndef TILEITEM_H
#define TILEITEM_H

#include <QGraphicsRectItem>
#include <QBrush>
#include <QColor>
#include <QPen>

#include "world.h"

// multi-inheritance in Qt
// http://www.setnode.com/blog/qt-staticmetaobject-is-not-a-member-of/
class TileItem : public QObject, public QGraphicsRectItem, public Tile
{
    Q_OBJECT

public:
    explicit TileItem(int x, int y, float color);
    virtual ~TileItem();

private:

};

#endif // TILEITEM_H
