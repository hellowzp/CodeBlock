#include "assetlabel.h"
#include <QDrag>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QDebug>
#include "assetsframe.h"

AssetLabel::AssetLabel()
{

    this->setFixedSize(55, 55);
    this->setAcceptDrops(true);

    this->_item = 0;

}

AssetLabel::~AssetLabel()
{
    qDebug() << "dtor of AssetLabel";
}

void AssetLabel::setType(int type)
{
    _type = type;

    if (_type == ASSERT_TYPE_EQUIPMENT_SWORD) {
        this->setText("Sword");
    } else if (_type == ASSERT_TYPE_EQUIPMENT_ARMOR) {

        this->setText("Armor");
    } else if (_type == ASSERT_TYPE_EQUIPMENT_BOOT) {

        this->setText("Boot");
    }
}

int AssetLabel::getType() const
{
    return _type;
}

void AssetLabel::setItem(int item)
{
    if (item == 0) {

        this->setPixmap(QPixmap());
        _item = item;

        if (_type == ASSERT_TYPE_EQUIPMENT_SWORD || _type == ASSERT_TYPE_EQUIPMENT_ARMOR || _type == ASSERT_TYPE_EQUIPMENT_BOOT)
            this->_frame->equipmentChanged();

        if (_type == ASSERT_TYPE_EQUIPMENT_SWORD) {
            this->setText("Sword");
        } else if (_type == ASSERT_TYPE_EQUIPMENT_ARMOR) {

            this->setText("Armor");
        } else if (_type == ASSERT_TYPE_EQUIPMENT_BOOT) {

            this->setText("Boot");
        }

        return;
    }

    if (_type == ASSERT_TYPE_ENERGY)
        if (item > 2) return;

    if (_type == ASSERT_TYPE_EQUIPMENT_SWORD)
        if (item != ASSERT_ITEM_SWORD) return;

    if (_type == ASSERT_TYPE_EQUIPMENT_ARMOR)
        if (item != ASSERT_ITEM_ARMOR) return;

    if (_type == ASSERT_TYPE_EQUIPMENT_BOOT)
        if (item != ASSERT_ITEM_BOOT) return;

    //then we set the item

    if (item == ASSERT_ITEM_HEALTH) {

        this->setPixmap(QPixmap(":/world/energy-icon.png"));
    } else if (item == ASSERT_ITEM_MAGIC) {

         this->setPixmap(QPixmap(":/world/magic-icon.png"));
    } else if (item == ASSERT_ITEM_SWORD) {

        this->setPixmap(QPixmap(":/world/sword-icon.png"));
    } else if (item == ASSERT_ITEM_ARMOR) {

        this->setPixmap(QPixmap(":/world/armor-icon.png"));
    } else if (item == ASSERT_ITEM_BOOT) {

        this->setPixmap(QPixmap(":/world/boot-icon.png"));
    }

    _item = item;

}

int AssetLabel::getItem() const
{
    return _item;
}

void AssetLabel::setFrame(AssetsFrame *frame)
{
    _frame = frame;
}

void AssetLabel::mousePressEvent(QMouseEvent *ev)
{
    if (_item == 0)
        return;

    QDrag *drag = new QDrag(this);
    drag->setObjectName("assert drag");

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << _item;
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/asset_item", itemData);

    _frame->setDragSource(this);

    drag->setMimeData(mimeData);

    drag->exec();


}

void AssetLabel::dragEnterEvent(QDragEnterEvent *event)
{


    if (event->mimeData()->hasFormat("application/asset_item")) {


        if (_item == 0) {

            QMimeData const *mimeData = event->mimeData();
            QByteArray itemData = mimeData->data("application/asset_item");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);
            int item;
            dataStream >> item;

            if (_type == ASSERT_TYPE_ENERGY)
                if (item > 2) return;
            if (_type == ASSERT_TYPE_EQUIPMENT_SWORD)
                if (item != ASSERT_ITEM_SWORD) return;
            if (_type == ASSERT_TYPE_EQUIPMENT_BOOT)
                if (item != ASSERT_ITEM_BOOT) return;
            if (_type == ASSERT_TYPE_EQUIPMENT_ARMOR)
                if (item != ASSERT_ITEM_ARMOR) return;


            event->accept();
            this->setItem(item);


        }
    }

}

void AssetLabel::dragLeaveEvent(QDragLeaveEvent *event)
{

        event->accept();
        this->setItem(0);

}

void AssetLabel::dropEvent(QDropEvent *event)
{


    if (event->mimeData()->hasFormat("application/asset_item")) {

        event->accept();
        AssetLabel *label = _frame->getDragSource();
        label->setItem(0);

        if (_type == ASSERT_TYPE_EQUIPMENT_SWORD || _type == ASSERT_TYPE_EQUIPMENT_ARMOR || _type == ASSERT_TYPE_EQUIPMENT_BOOT)
            this->_frame->equipmentChanged();



    }

}
