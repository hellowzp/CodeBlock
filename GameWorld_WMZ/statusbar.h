#ifndef STATUSBAR_H
#define STATUSBAR_H


#include <QGraphicsRectItem>



class StatusBar : public QGraphicsRectItem
{
public:
    enum StatusBarType {
        HealthBar, MagicBar
    };

    StatusBar(StatusBarType type, QGraphicsItem *parent);
    ~StatusBar();
    void setPercentage(int percentage);



private:
    QGraphicsRectItem _statusFrame;
    int _currentPecentage;
    const int _totalPencentage = 100;
    const int _barWidth = 50;
    const int _barHight = 5;
};

#endif // STATUSBAR_H
