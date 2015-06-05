GameWorld
=========

I didn't notice the game rules that enemies can not move and the existing class
of Enemy and Protagonist at first. My original idea is that both hero and enenmy will 
be able to move, so I create a MoveableSpriteItem to be inherited for both, 
and also a StaticSpriteItem class used for items like health packs or maybe 
other types for extension in the future.

MoveableSpriteItem and StaticSpriteItem both extends SpriteItem, which then extends:
	public QObject, public Tile, public QGraphicsPixmapItem

