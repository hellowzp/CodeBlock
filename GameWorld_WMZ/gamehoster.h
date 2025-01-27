#ifndef GAMEHOSTER_H
#define GAMEHOSTER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QThread>
#include <QTcpServer>
#include <QPoint>


#define BROADCAST_PORT 45454
#define TCP_CONNECTION_PORT 45456

#define TCP_DATA_TYPE_CON_ID 34
#define TCP_DATA_TYPE_NEED_CREATE_WORLD 35
#define TCP_DATA_TYPE_WORLD_CREATED 36
#define TCP_DATA_TYPE_START_GAME 38
#define TCP_DATA_TYPE_PLAYER 246 //from client to server
#define TCP_DATA_TYPE_QUERY_PLAYERS 299 // from server to client
#define TCP_DATA_TYPE_HERO_REQUEST 300
#define TCP_DATA_TYPE_NEW_HERO 302
#define TCP_DATA_TYPE_HERO_MOVE 303
#define TCP_DATA_TYPE_HERO_MOVING_STOPED 305
#define TCP_DATA_TYPE_OPPONENT_REQUEST 311
#define TCP_DATA_TYPE_NEW_OPPONENT 322
#define TCP_DATA_TYPE_OPPONENT_MOVED 324
#define TCP_DATA_TYPE_HERO_MAGIC 333
#define TCP_DATA_TYPE_OPPONENT_INJURED 334
#define TCP_DATA_TYPE_HERO_INJURED 335
#define TCP_DATA_TYPE_INJURE_HERO 336
#define TCP_DATA_TYPE_ROUND_OVER 337
#define TCP_DATA_TYPE_ROUND_OVER_CONFIRMED 338
#define TCP_DATA_TYPE_NEW_GIFT 339
#define TCP_DATA_TYPE_GIFT_PICKED 340
#define TCP_DATA_TYPE_GAME_OVER 365
#define TCP_DATA_TYPE_HERO_HEALTH_INCREASED 378
#define TCP_DATA_TYPE_HERO_MAGIC_INCREASED 379
#define TCP_DATA_TYPE_HERO_MAGIC_USED 399
#define TCP_DATA_TYPE_PAUSE_GAME 400
#define TCP_DATA_TYPE_RESUME_GAME 401

#define PLAYER_ROLE_MAGE 1
#define PLAYER_ROLE_WORRIOR 2
#define PLAYER_ROLE_PRIEST 3

#define ROOM_STATE_IDLE 1
#define ROOM_STATE_PLAYING 2

#define CLIENT_STATE_CONNECTED 1
#define CLIENT_STATE_ID_RECEIVED 2
#define CLIENT_STATE_WORLD_READY 3
#define CLIENT_STATE_FIRST_ROUND_OVER 4
#define CLIENT_STATE_SECOND_ROUND_OVER 5



struct Room {

    QHostAddress senderAddress;
    QString roomName;
    int8_t map;
    int8_t maxPlayerNum;
    int8_t currentPlayerNum;
    int8_t state;
};

struct Player {

    int16_t id;
    QString playerName;
    int16_t role;
    int16_t state;

    int getByteSize() {
        return sizeof(int16_t) * 2 + 4 + playerName.size() * 2;
    }
};


struct HeroModel {

    int16_t id;
    QString name;
    int16_t role;
    int16_t x;
    int16_t y;
    int8_t healthPercentage;
    int8_t magicPercentage;

    int getByteSize() {
        return sizeof(int16_t) * 4 + sizeof(int8_t) * 2 + 4 + name.size() * 2;
    }
};

struct OpponentModel {


    int16_t id;
    int16_t type;
    int16_t x;
    int16_t y;
    int8_t healthPecentage;

    int getByteSize() {

        return sizeof(int16_t) * 4 + sizeof(int8_t);
    }
};

struct GiftModel {


    int16_t id;
    int16_t x;
    int16_t y;
    int16_t type;

    int getByteSize() {

        return sizeof(int16_t) * 4;
    }
};

QDataStream & operator >> ( QDataStream & stream, GiftModel & gift );
QDataStream & operator << ( QDataStream & stream, GiftModel & gift );
QDataStream & operator >> ( QDataStream & stream, OpponentModel & opponent );
QDataStream & operator << ( QDataStream & stream, OpponentModel & opponent );
QDataStream & operator >> ( QDataStream & stream, HeroModel & hero );
QDataStream & operator << ( QDataStream & stream, HeroModel & hero );
QDataStream & operator >> ( QDataStream & stream, Room & room );
QDataStream & operator << ( QDataStream & stream, Room & room );
QDataStream & operator >> ( QDataStream & stream, Player & player );
QDataStream & operator << ( QDataStream & stream, Player & player );

class GameHoster : public QThread
{
    Q_OBJECT
public:

    static void CreateHoster(Room room);
    static void DestoryHoser();
    static GameHoster* Instance();
    ~GameHoster();

signals:

    void needSendBroadcast();

public slots:

private slots:

    void sendBroadcast();
    void newClientConnection();
    void readData();
    void startGame();

private:
    explicit GameHoster(Room &room);

    void run();

    QTcpServer *_server;
    QVector<QTcpSocket *> _clientSockets;

    bool _isBroadcasting = true;

    QUdpSocket *_broadcaster;
    QTimer *_broadcastTimer;
    Room _room;
    int _currentRound;

    int _worldWidth = 0;
    int _worldHeight = 0;

    QVector<Player> _players;
    QVector<int> _clientStates;
    QVector<QPoint> _coorsBuffer;
    QVector<HeroModel> _heros;
    QVector<OpponentModel> _opponents;
    QVector<GiftModel> _gifts;

    static GameHoster *_instance;

    void assignRoles(int round);
    bool checkIfRoundOver();
    void generateGifts(OpponentModel &opponent);
};

#endif // GAMEHOSTER_H
