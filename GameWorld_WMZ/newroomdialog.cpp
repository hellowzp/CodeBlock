#include "newroomdialog.h"
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include "gamehoster.h"
#include <QNetworkInterface>
#include "gameclient.h"
#include "gamehostdialog.h"
#include <QLabel>

NewRoomDialog::NewRoomDialog()
{

    this->setWindowTitle("Create Room");
    QHBoxLayout *layout = new QHBoxLayout();
    this->setLayout(layout);
    layout->setSpacing(20);

    //-------------------input room name ----------------
    QVBoxLayout *roomNameLineLayout = new QVBoxLayout();
    QLabel *roomNameLineLabel = new QLabel();
    roomNameLineLabel->setText("Input Room Name");

    _roomNameLineEdit = new QLineEdit();
    roomNameLineLayout->addWidget(roomNameLineLabel);
    roomNameLineLayout->addWidget(_roomNameLineEdit);
    layout->addLayout(roomNameLineLayout);

    //---------------------select max player------------
    QVBoxLayout *maxPlayerBoxLayout = new QVBoxLayout();
    QLabel *maxPlayerLabel = new QLabel();
    maxPlayerLabel->setText("Select max Player");
    _maxPlayerNumComboBox = new QComboBox();
    _maxPlayerNumComboBox->addItem("2", 2);
    _maxPlayerNumComboBox->addItem("3", 3);
    _maxPlayerNumComboBox->addItem("4", 4);
    maxPlayerBoxLayout->addWidget(maxPlayerLabel);
    maxPlayerBoxLayout->addWidget(_maxPlayerNumComboBox);
    layout->addLayout(maxPlayerBoxLayout);

    //-----------------select map----------------------
    QVBoxLayout *mapBoxLayout = new QVBoxLayout();
    QLabel *mapBoxLabel = new QLabel();
    mapBoxLabel->setText("Select map");
    _mapComboBox = new QComboBox();
    _mapComboBox->addItem("map 1", 1);
    _mapComboBox->addItem("map 2", 2);
    _mapComboBox->addItem("map 3", 3);
    mapBoxLayout->addWidget(mapBoxLabel);
    mapBoxLayout->addWidget(_mapComboBox);
    layout->addLayout(mapBoxLayout);

    //------------------create-------------------
    QPushButton *createButton = new QPushButton();
    createButton->setText("Create");
    QObject::connect(createButton, SIGNAL(clicked()), this, SLOT(CreateButtonCreated()));

    createButton->setFixedSize(100,50);
    layout->addWidget(createButton);

    this->setFixedSize(500,85);

}

NewRoomDialog::~NewRoomDialog()
{
    qDebug() << "dtor of NewRoomDialog";

    delete _roomNameLineEdit;
    delete _maxPlayerNumComboBox;
    delete _mapComboBox;
}

void NewRoomDialog::setMyName(QString name)
{
    this->_myName = name;
}

void NewRoomDialog::setMainWindow(MainWindow *mainWindow)
{
    this->_mainWindow = mainWindow;
}

void NewRoomDialog::CreateButtonCreated()
{

    Room room;
    room.roomName = _roomNameLineEdit->text();
    room.maxPlayerNum = _maxPlayerNumComboBox->itemData(_maxPlayerNumComboBox->currentIndex()).toInt();
    room.map = _mapComboBox->itemData(_mapComboBox->currentIndex()).toInt();
    room.currentPlayerNum = 1;
    room.state = ROOM_STATE_IDLE;
    QNetworkInterface interface;
    for (QHostAddress address : interface.allAddresses()) {

        if (address.toIPv4Address() && address != QHostAddress::LocalHost) {
               room.senderAddress = address;
               break;
        }
    }

    Player player;
    player.id = 0;
    player.playerName = _myName;
    player.role = PLAYER_ROLE_MAGE;

    qDebug() << "come here";

    GameHoster::CreateHoster(room);
    qDebug() << "come here";
    GameClient::CreateClient(room, player);
    GameClient *client = GameClient::Instance();
    QObject::connect(client, SIGNAL(needToCreateWorld(int)), _mainWindow, SLOT(createWorld(int)));
    QObject::connect(_mainWindow, SIGNAL(worldCreated()), client, SLOT(worldCreated()));
    QObject::connect(_mainWindow, SIGNAL(pauseGame()), client, SLOT(pauseGame()));
    QObject::connect(_mainWindow, SIGNAL(resumeGame()), client, SLOT(resumeGame()));
    QObject::connect(client, SIGNAL(needToGameOver(bool)), _mainWindow, SLOT(gameOver(bool)));


    //enable buttons as a host
    _mainWindow->enableButtons();



    //create a host dialog and enable the start button
    GameHostDialog *dialog = new GameHostDialog(true);
    dialog->show();



    this->close();
    delete this;
}
