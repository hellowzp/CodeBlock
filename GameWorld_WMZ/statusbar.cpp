#include "statusbar.h"
#include <QDebug>

#include <QBrush>

StatusBar::StatusBar(StatusBarType type, QGraphicsItem *parent)
{
    this->setParentItem(parent);
    _statusFrame.setParentItem(this);

    _statusFrame.setRect(0, 0, _barWidth, _barHight);
    if (type == HealthBar) {

        this->setBrush(QBrush(QColor(255 ,0, 0)));
    } else if (type == MagicBar) {
        this->setBrush(QBrush(QColor(0, 0, 255)));
    }
}

StatusBar::~StatusBar()
{
    qDebug() << "dtor of StatusBar";
}

void StatusBar::setPercentage(int percentage)
{
    if (0 <= percentage &&  percentage <= 100) {

        _currentPecentage = percentage;
        this->setRect(0, 0, _currentPecentage * _barWidth/ _totalPencentage, _barHight);
    }
}
