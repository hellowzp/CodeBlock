#include "SpriteItem.h"
#include "WorldView.h"

#include <QDebug>

SpriteItem::SpriteItem(const Tile &tile):
    Tile(tile.getXPos(), tile.getYPos(), tile.getValue())
{
    setID();
    setPos( TOPLEFT_XOFFSET + getXPos() * WorldView::TILE_SIZE,
            TOPLEFT_YOFFSET + getYPos() * WorldView::TILE_SIZE );

    propertyView = new QProgressBar();
    propertyView->setObjectName("propertyView");
    propertyView->setMinimum(0);
    propertyView->setMaximum(100);
    propertyView->setMaximumWidth(34);
    propertyView->setMaximumHeight(12);
    propertyView->setGeometry(pos().x(),pos().y()-15,32,12);

    propertyView->setTextVisible(false);
    hidePropertyView();

    setAcceptHoverEvents(true);
    qDebug() << "SpriteItem()" << ID;
}

SpriteItem::~SpriteItem()
{
    delete(propertyView);
    qDebug() << "~SpriteItem()" << ID;
}

//void SpriteItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
//    updatePropertyView();
//}

//void SpriteItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event){
//    hidePropertyView();
//}

void SpriteItem::updatePropertyView() {
    propertyView->setValue(getValue());
    propertyView->setVisible(true);
}

void SpriteItem::hidePropertyView() {
    propertyView->setVisible(false);
}

//SpriteItem::SpriteItem(const SpriteItem& other):
//    Tile(other.getXPos(),other.getYPos(),other.getValue())
//{
//    //copy ctor
//}

//SpriteItem& SpriteItem::operator=(const SpriteItem& rhs)
//{
//    if (this == &rhs) return *this; // handle self assignment
//    //assignment operator
//    return *this;
//}

//int SpriteItem::getDistance(const SpriteItem& sprite) const {
//    int dx = getXPos()-sprite.getXPos();
//    int dy = getYPos()-sprite.getYPos();
//    return (dx<0?-dx:dx) + (dy<0?-dy:dy);
//}

//int SpriteItem::getDirection(const SpriteItem& sprite) {
//    int dx = getXPos()-sprite.getXPos();
//    int dy = getYPos()-sprite.getYPos();

//    return dx-dy;
//}

