#include "DisplayView.h"

#include <QLabel>
#include <QGridLayout>
#include <QDebug>

DisplayView::DisplayView(MainWindow* mainWindow)
{
    this->mainWindow = mainWindow;

    QGridLayout* gridLayout = new QGridLayout;

    QFont font;
    font.setPointSize(20);
    font.setBold(false);
    QLabel* healthLabel = new QLabel(QString("Health"));
    healthLabel->setFont(font);

    healthBar = new QProgressBar();
    healthBar->setMinimum(0);
    healthBar->setMaximum(100);
    healthBar->setValue(100);

    QLabel* energyLabel = new QLabel(QString("Energy"));
    energyLabel->setFont(font);

    energyBar = new QProgressBar();
    energyBar->setMinimum(0);
    energyBar->setMaximum(100);
    energyBar->setValue(100);

    gridLayout->addWidget(healthLabel,0,0);
    gridLayout->addWidget(healthBar,0,1);
    gridLayout->addWidget(energyLabel,1,0);
    gridLayout->addWidget(energyBar,1,1);

//    rLayout->setAlignment(Qt::AlignRight);

//    QHBoxLayout* mainLayout = new QHBoxLayout;
//    mainLayout->addLayout(rLayout);
//    mainLayout->addStretch();
//    mainLayout->addLayout(rLayout);
    this->setLayout(gridLayout);
    this->setFixedWidth(16*34);
    this->setStyleSheet("QProgressBar { border: 1px solid grey; border-radius: 5px; text-align: center; }");
//    this->setStyleSheet("QProgressBar::chunk { background-color: #CD96CD; width: 10px; margin: 0.5px; }");

}

DisplayView::~DisplayView() {

}

void DisplayView::changeHealth(int health) {
    healthBar->setValue(health);
    qDebug() << "health changed!"  << health;
}

void DisplayView::changeEnergy(int energy) {
    energyBar->setValue(energy);
    qDebug() << "energy changed!"  << energy;
}



