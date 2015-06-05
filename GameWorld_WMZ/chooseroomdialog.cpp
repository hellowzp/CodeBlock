#include "chooseroomdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QScrollArea>
#include "roomframe.h"
#include <QPushButton>
#include <iostream>
#include "newroomdialog.h"
#include "gamehoster.h"
#include <QNetworkInterface>
#include <QListWidgetItem>

ChooseRoomDialog::ChooseRoomDialog()

{

    QVBoxLayout *mainLayout = new QVBoxLayout();
    this->setLayout(mainLayout);
    this->setWindowTitle("Choose Room");
    //create the upper part of the layout

    QLineEdit *playerNameLineEdit = new QLineEdit();
    QObject::connect(playerNameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(myNameChanged(QString)));
    QLabel *playerNameLabel = new QLabel();
    playerNameLabel->setText("Your Name: ");

    QHBoxLayout *upperLayout = new QHBoxLayout();
    upperLayout->addWidget(playerNameLabel);
    upperLayout->addWidget(playerNameLineEdit);

    mainLayout->addLayout(upperLayout);

    //create the middle part the layout

    this->_roomListLayout = new QListWidget();
    this->_roomListLayout->addItem("  Host                  IP                  Current         Max         Map ");
    mainLayout->addWidget(_roomListLayout);

    //create the lower part of the layout

    QHBoxLayout *lowerLayout = new QHBoxLayout();

    QPushButton *createRoomButton = new QPushButton();
    createRoomButton->setText("Creat a room");
    QObject::connect(createRoomButton, SIGNAL(clicked()),this, SLOT(createRoomClicked()));

    lowerLayout->addWidget(createRoomButton);

    mainLayout->addLayout(lowerLayout);

    //set up the broadcast receiver
    this->_broadcastReceiver = new QUdpSocket();
    this->_broadcastReceiver->bind(45454, QUdpSocket::ShareAddress);
    QObject::connect(_broadcastReceiver, SIGNAL(readyRead()), this, SLOT(broadcastReceived()));


    this->setFixedSize(500,300);
}

ChooseRoomDialog::~ChooseRoomDialog()
{
    qDebug() << "dtor of ChooseRoomDialog";

    delete _roomListLayout;
    _broadcastReceiver->close();
    delete _broadcastReceiver;

    for (RoomFrame *frame : _roomFrames) delete frame;
}

void ChooseRoomDialog::setMainWidow(MainWindow *mainWindow)
{
    _mainWindow = mainWindow;
}



void ChooseRoomDialog::createRoomClicked()
{

    //create middle layout
    NewRoomDialog *newRoomDialog = new NewRoomDialog();
    newRoomDialog->show();
    newRoomDialog->setMyName(_myName);
    newRoomDialog->setMainWindow(_mainWindow);

    //close and remove this dialog
    this->close();
    delete this;
}


void ChooseRoomDialog::broadcastReceived()
{

    while (this->_broadcastReceiver->hasPendingDatagrams()) {

        QByteArray broadcastData;
        broadcastData.resize(_broadcastReceiver->pendingDatagramSize());
        this->_broadcastReceiver->readDatagram(broadcastData.data(), 1000);
        QDataStream dataStream(&broadcastData, QIODevice::ReadOnly);
        Room room;
        dataStream >> room;

        bool roomExist = false;
        for(Room & myRoom : _rooms) {
            if (room.senderAddress == myRoom.senderAddress) {
                roomExist = true;
                break;
            }
        }
        if (!roomExist) {
            this->_rooms.push_back(room);
            RoomFrame *roomFrame = new RoomFrame(room);
            roomFrame->setMyName(_myName);
            roomFrame->setMainWindow(_mainWindow);
            _roomFrames.push_back(roomFrame);
            QObject::connect(roomFrame, SIGNAL(closeRoomDialog()), this, SLOT(close()));

            QListWidgetItem *item = new QListWidgetItem();
            item->setSizeHint(QSize(item->sizeHint().height(),50));
            this->_roomListLayout->addItem(item);
            this->_roomListLayout->setItemWidget(item,roomFrame);

        } else {

            for (int i = 0; i < _rooms.size(); i++) {

                if (_rooms[i].senderAddress == room.senderAddress) {

                    _roomFrames[i]->updataFrame(room);
                    break;
                }
            }
        }

        std::cout << "broadcast received " << room.roomName.toStdString() <<std::endl;
    }
}

void ChooseRoomDialog::myNameChanged(QString name)
{
    _myName = name;
    for (RoomFrame *frame : _roomFrames) {

        frame->setMyName(name);
    }
}
