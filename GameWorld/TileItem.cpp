#include "TileItem.h"
#include "WorldView.h"

/* THIS will nor compile because it will call the
 * default constructor of all its superclasses,
 * while Tile doesn't have such a constructor

    TileItem::TileItem() {}
*/

TileItem::TileItem(int x, int y, float color):
    Tile(x,y,color)
{
    this->setRect( x * WorldView::TILE_SIZE, y * WorldView::TILE_SIZE,
                   WorldView::TILE_SIZE, WorldView::TILE_SIZE );
    this->setBrush(QBrush(QColor(0, 255.0f * this->getValue(), 0)));
    this->setPen(Qt::NoPen);
}


TileItem::~TileItem() {

}

//TileItem::getWid
