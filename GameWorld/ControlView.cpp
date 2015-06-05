#include "ControlView.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>

ControlView::ControlView(MainWindow* mainWindow):
    paused(true), stopped(false)
{
    this->mainWindow = mainWindow;

    QHBoxLayout* hLayout = new QHBoxLayout;

    QFont font;
    font.setPointSize(15);
    font.setBold(false);

    QLabel* heuristic = new QLabel(QString("heuristic"));
    heuristic->setFont(font);

    hSlider = new QSlider(Qt::Horizontal);
    hSlider->setMinimum(0);
    hSlider->setMaximum(10);
    hSlider->setValue(10);
    hSlider->setFixedWidth(80);
    hSlider->setCursor(QCursor(Qt::PointingHandCursor));

    QLabel* fps = new QLabel(QString("FPS"));
    fps->setFont(font);

    fpsSlider = new QSlider(Qt::Horizontal);
    fpsSlider->setMinimum(1);
    fpsSlider->setMaximum(10);
    fpsSlider->setValue(3);
    fpsSlider->setFixedWidth(80);
    fpsSlider->setCursor(QCursor(Qt::PointingHandCursor));

    itrct = new QCheckBox("Enable Interaction");

    pause = new QPushButton;
    pause->setText("Start");
    pause->setFixedWidth(40);

    stop = new QPushButton;
    stop->setText("Stop");
    stop->setFixedWidth(40);
    stop->setDisabled(true);

    hLayout->addWidget(heuristic);
    hLayout->addWidget(hSlider);
    hLayout->addWidget(fps);
    hLayout->addWidget(fpsSlider);

    hLayout->addWidget(itrct);
    hLayout->addWidget(pause);
    hLayout->addWidget(stop);

    this->setLayout(hLayout);
    this->setFixedWidth(34*16+8);

    QObject::connect(pause, SIGNAL(clicked()), this, SLOT(pauseClicked()));
    QObject::connect(stop,  SIGNAL(clicked()), this, SLOT(stopClicked()) );
    QObject::connect(itrct, SIGNAL(clicked()), this, SLOT(itrctClicked()));

}

ControlView::~ControlView()
{

}
