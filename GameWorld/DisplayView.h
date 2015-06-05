#ifndef DISPLAYVIEW_H
#define DISPLAYVIEW_H

#include <QWidget>
#include <QProgressBar>

class MainWindow;

class DisplayView : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayView(MainWindow *mainWindow);
    ~DisplayView();

private:
    MainWindow* mainWindow;

    QProgressBar* healthBar;
    QProgressBar* energyBar;

public slots:
    void changeHealth(int health);
    void changeEnergy(int energy);
    void resetControls(){
        healthBar->setValue(100);
        energyBar->setValue(100);
    }

};

#endif // DISPLAYVIEW_H
