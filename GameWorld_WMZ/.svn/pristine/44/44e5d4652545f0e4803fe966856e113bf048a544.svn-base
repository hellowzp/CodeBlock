#ifndef PLAYERFRAME_H
#define PLAYERFRAME_H

#include <QFrame>
#include <QComboBox>
#include <QLabel>
#include "gamehoster.h"

class PlayerFrame : public QFrame
{
    Q_OBJECT
public:
    explicit PlayerFrame(Player &player, bool enable);
    ~PlayerFrame();
    Player &getPlayer();
    void updataFrame(Player &player);

signals:

    void playerChanged(Player);

public slots:

private slots:
    void roleChanged(int index);

private:
    QComboBox *_roleComboBox;
    QLabel *_nameLabel;
    Player _player;

};

#endif // PLAYERFRAME_H
