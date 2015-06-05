#ifndef TCPCONNECTIONDIALOG_H
#define TCPCONNECTIONDIALOG_H

#include <QDialog>
#include <QUdpSocket>
#include "gamehoster.h"
#include "roomframe.h"
#include "mainwindow.h"
#include <QListWidget>

class QLabel;
class QPushButton;
class QScrollArea;
class QVBoxLayout;

class ChooseRoomDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ChooseRoomDialog();
    ~ChooseRoomDialog();
    void setMainWidow(MainWindow *mainWindow);

signals:

public slots:

private slots:
    void createRoomClicked();
    void broadcastReceived();
    void myNameChanged(QString name);

private:

    QListWidget *_roomListLayout;
    QUdpSocket *_broadcastReceiver;
    QList<Room> _rooms;
    QVector<RoomFrame *> _roomFrames;

    QString _myName;
    MainWindow *_mainWindow;


};

#endif // TCPCONNECTIONDIALOG_H
