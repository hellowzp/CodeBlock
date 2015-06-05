#ifndef SPRITEITEM_H
#define SPRITEITEM_H

#include <QGraphicsPixmapItem>
#include <QColor>
#include <QProgressBar>

#include "world.h"
#include "TileItem.h"

class WorldView;

class SpriteItem : public QObject, public Tile, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit SpriteItem(const Tile& tile);
    virtual ~SpriteItem() =0; //force it to be an interface

//    SpriteItem(const SpriteItem& other);
//    SpriteItem& operator=(const SpriteItem& other);

//    enum class Direction : char { N = 1, NE, E, SE, S, SW, W, NW };

    int getID() const { return ID; }
    WorldView* getWorldView() const { return worldView; }
    void setWorldView(WorldView* view) { worldView = view; }

    QProgressBar* getPropertyView() { return propertyView; }
    void hidePropertyView();
    void updatePropertyView();

    /*
    friend QDebug& operator<< (QDebug& qdg, const SpriteItem& sprite) {
        qdg << sprite.getXPos() << sprite.getYPos() << sprite.getValue() << sprite.ID;
        return qdg;
    } */

    static const int TOPLEFT_XOFFSET = 1;
    static const int TOPLEFT_YOFFSET = 1;

protected:
    WorldView* worldView;

//    void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
//    void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

private:
    int ID;   //make it private and no setter method, equivalent to const
    void setID() { static int id = 0; id++; ID = id; }

    QProgressBar* propertyView;

};

#endif // SPRITEITEM_H
