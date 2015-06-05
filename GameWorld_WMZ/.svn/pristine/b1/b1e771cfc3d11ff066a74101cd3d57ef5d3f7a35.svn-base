#ifndef ASSERTLABEL_H
#define ASSERTLABEL_H


#define ASSERT_TYPE_ENERGY 1
#define ASSERT_TYPE_EQUIPMENT_SWORD 2
#define ASSERT_TYPE_EQUIPMENT_ARMOR 3
#define ASSERT_TYPE_EQUIPMENT_BOOT 4
#define ASSERT_TYPE_GENERAL 5

#define ASSERT_ITEM_HEALTH 1
#define ASSERT_ITEM_MAGIC 2
#define ASSERT_ITEM_SWORD 3
#define ASSERT_ITEM_ARMOR 4
#define ASSERT_ITEM_BOOT 5

#include <QLabel>

class AssetsFrame;


class AssetLabel : public QLabel
{
    Q_OBJECT
public:
    explicit AssetLabel();
    ~AssetLabel();

    void setType(int type);
    int getType() const;
    void setItem(int item);
    int getItem() const;
    void setFrame(AssetsFrame *frame);

signals:

public slots:


private:

    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    int _type;
    int _item;

    AssetsFrame *_frame;

};

#endif // ASSERTLABEL_H
