#include "assetsframe.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QGroupBox>
#include <QKeyEvent>

AssetsFrame::AssetsFrame()
{   

    QVBoxLayout *mainlayout = new QVBoxLayout();

     this->setLayout(mainlayout);

 //------------energy-------------------------------

     _energyLayout = new QHBoxLayout();

     QLabel *_energy_label = new QLabel();
     _energy_label->setText("Energy");
     _energy_label->setAlignment(Qt::AlignLeft);

     mainlayout->addWidget(_energy_label);
     QGroupBox *_energyLayout_box = new QGroupBox();

     _energyLayout_box->setAlignment(Qt::AlignCenter);
     _energyLayout_box->setLayout(_energyLayout);
     _energyLayout_box->setStyleSheet("border : 2px solid grey;");

     QHBoxLayout *x1 = new QHBoxLayout();

     mainlayout->addLayout(x1);

     x1->addWidget(_energyLayout_box);

     for (int i = 0; i < 4; i++) {

         AssetLabel *label = new AssetLabel();

         _energyLayout->addWidget(label);

         label->setType(ASSERT_TYPE_ENERGY);
         label->setFrame(this);
         label->setAlignment(Qt::AlignCenter);
         _energyLabels.push_back(label);

     }

 //--------------euqipment-------------------------

     QLabel *_equipment_label = new QLabel();
     _equipment_label->setText("Equipment");
     _equipment_label->setAlignment(Qt::AlignLeft);
     mainlayout->addWidget(_equipment_label);

     _equipmentLayout = new QHBoxLayout();
     QGroupBox *_equipmentLayout_box = new QGroupBox();

     _equipmentLayout_box->setAlignment(Qt::AlignCenter);

     _equipmentLayout_box->setLayout(_equipmentLayout);

     _equipmentLayout_box->setStyleSheet("border : 2px solid grey;");

     QHBoxLayout *x2 = new QHBoxLayout();

     mainlayout->addLayout(x2);

     x2->addWidget(_equipmentLayout_box);

     AssetLabel *swordLabel = new AssetLabel();

     swordLabel->setType(ASSERT_TYPE_EQUIPMENT_SWORD);
     swordLabel->setFrame(this);
     _swordLabel = swordLabel;

     _equipmentLayout->addWidget(swordLabel);

     AssetLabel *armorLabel = new AssetLabel();

     armorLabel->setType(ASSERT_TYPE_EQUIPMENT_ARMOR);
     armorLabel->setFrame(this);
     _armorLabel = armorLabel;

     _equipmentLayout->addWidget(armorLabel);

     AssetLabel *bootLabel = new AssetLabel();

     bootLabel->setType(ASSERT_TYPE_EQUIPMENT_BOOT);
     bootLabel->setFrame(this);
     _bootLabel = bootLabel;

     _equipmentLayout->addWidget(bootLabel);

 //----------------------assetsLayout---------------------------

     QLabel *_assets_label = new QLabel();

     _assets_label->setText("Assets");

     _assets_label->setAlignment(Qt::AlignLeft);

     mainlayout->addWidget(_assets_label);

     _assetsLayout = new QGridLayout();

     QGroupBox *_assetLayout_box = new QGroupBox();

     _assetLayout_box->setAlignment(Qt::AlignCenter);

     _assetLayout_box->setLayout(_assetsLayout);

     _assetLayout_box->setStyleSheet("border : 2px solid grey;");

     QHBoxLayout *x3 = new QHBoxLayout();

     mainlayout->addLayout(x3);

     x3->addWidget(_assetLayout_box);

     mainlayout->addLayout(x3);

     for (int i = 0; i < 4; i ++) {

         for (int j = 0; j < 4; j++) {

             AssetLabel *label = new AssetLabel();

             _assetsLayout->addWidget(label, i, j);

             _assetsLabels.push_back(label);
             label->setType(ASSERT_TYPE_GENERAL);

             label->setFrame(this);

         }

     }

     this->setFixedHeight(500);

}

AssetsFrame::~AssetsFrame()
{
    delete _swordLabel;
    delete _bootLabel;
    delete _armorLabel;

    for (AssetLabel *label : _energyLabels) {

        delete label;
    }

    for (AssetLabel *label : _assetsLabels) {

        delete label;
    }
}

void AssetsFrame::setDragSource(AssetLabel *source)
{
    _dragSource = source;
}

AssetLabel *AssetsFrame::getDragSource() const
{
    return _dragSource;
}

void AssetsFrame::equipmentChanged()
{
    bool sword = (_swordLabel->getItem() != 0);
    bool armor = (_armorLabel->getItem() != 0);
    bool boot = (_bootLabel->getItem() != 0);

    qDebug() << _swordLabel->getItem() << " " << _armorLabel->getItem() << " " << _bootLabel->getItem();

    emit equipmentChanged(sword, armor, boot);
}

void AssetsFrame::clearLabels()
{
    _swordLabel->setItem(0);
    _bootLabel->setItem(0);
    _armorLabel->setItem(0);

    for (AssetLabel *label : _energyLabels) {

        label->setItem(0);
    }

    for (AssetLabel *label : _assetsLabels) {

        label->setItem(0);
    }
}

void AssetsFrame::pickGift(int type)
{
    for (AssetLabel *label : _assetsLabels) {

        if (label->getItem() == 0) {

            label->setItem(type);
            break;
        }
    }
}

void AssetsFrame::keyPressed(int key)
{
    if (key > 4 || key < 1) return;

    AssetLabel *label = _energyLabels[key - 1];

    if (label->getItem() != 0) {

        emit drinkEnergy(label->getItem());

        label->setItem(0);
    }
}

