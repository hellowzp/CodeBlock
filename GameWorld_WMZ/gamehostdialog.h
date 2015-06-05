#ifndef GAMEHOSTDIALOG_H
#define GAMEHOSTDIALOG_H

#include <QDialog>
#include "playerframe.h"
#include "gamehoster.h"
#include <QListWidget>

class QVBoxLayout;

class GameHostDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GameHostDialog(bool isEnable);
    ~GameHostDialog();
signals:

    void needToRequestOtherPlayers();

public slots:


    void otherPlayer(Player player);
    void myPlayer(Player player);
    void closeDialog();
private:
    QListWidget *_playerListLayout;
    PlayerFrame *_myFrame;

    Player _myPlayer;
    QVector<Player> _otherPlayers;
    QVector<PlayerFrame *> _otherPlayerFrames;

};

#endif // GAMEHOSTDIALOG_H
