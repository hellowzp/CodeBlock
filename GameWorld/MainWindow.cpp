#include "MainWindow.h"

#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QWidget* centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    centralWidget->setObjectName("Central widget");

    QVBoxLayout* layout = new QVBoxLayout;
    centralWidget->setLayout(layout);

    displayView = new DisplayView(this);
    worldView = new WorldView(this);
    controlView = new ControlView(this);
    layout->addWidget(displayView);
    layout->addWidget(worldView);
    layout->addWidget(controlView);

//    resize(700,750);
    setGeometry(280,30,34*16+8,680);

    /* centering on the screen
    //int width = frameGeometry().width();
    //int height = frameGeometry().height();
    int width = 650;
    int height = 680;

    QDesktopWidget wid;
    int screenWidth = wid.screen()->width();
    int screenHeight = wid.screen()->height();

    setGeometry((screenWidth/2)-(width/2),(screenHeight/2)-(height/2),width,height);
    */

    QObject::connect(worldView, SIGNAL(gameRestarted()), controlView, SLOT(resetControls()));
    QObject::connect(worldView, SIGNAL(gameRestarted()), displayView, SLOT(resetControls()));
    resetHeroConnections();

    QObject::connect(controlView, SIGNAL(pauseChanged(bool)), worldView, SLOT(togglePause(bool)));
    QObject::connect(controlView, SIGNAL(stopChanged(bool)),  worldView, SLOT(toggleStop(bool)));
    QObject::connect(controlView, SIGNAL(interactChanged(bool)), worldView, SLOT(changeInteraction(bool)));
    QObject::connect(controlView->hSlider, SIGNAL(valueChanged(int)), worldView, SLOT(changeheuristic(int)));
    QObject::connect(controlView->fpsSlider, SIGNAL(valueChanged(int)), worldView, SLOT(changeFPS(int)));

}

void MainWindow::resetHeroConnections(){
    QObject::connect(worldView->hero, SIGNAL(healthChanged(int)), displayView, SLOT(changeHealth(int)));
    QObject::connect(worldView->hero, SIGNAL(energyChanged(int)), displayView, SLOT(changeEnergy(int)));
}



MainWindow::~MainWindow() {
//    delete view;
}
