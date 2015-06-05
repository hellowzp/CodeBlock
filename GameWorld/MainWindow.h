#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QSizePolicy>
#include <QPushButton>

#include "WorldView.h"
#include "DisplayView.h"
#include "ControlView.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    WorldView*   worldView;
    ControlView* controlView;
    DisplayView* displayView;


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void resetHeroConnections();

signals:

public slots:

};


#endif // MAINWINDOW_H
