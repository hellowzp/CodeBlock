#include "MoveableSpriteItem.h"
#include "WorldView.h"
#include "MainWindow.h"
#include "PriorityQueue.h"

#include <cassert>

MoveableSpriteItem::MoveableSpriteItem(const Tile& tile):
    SpriteItem(tile)
{
    qDebug() << "MoveableSpriteItem::MoveableSpriteItem()" << getID();
}

MoveableSpriteItem::~MoveableSpriteItem()
{
    qDebug() << "MoveableSpriteItem::~MoveableSpriteItem()" << getID();
}

// x: column, y: row; both begins from 0
void MoveableSpriteItem::updatePosition(int x, int y) {
    assert(x>=0 && y>=0 && x<WorldView::MAP_ROWS && y<WorldView::MAP_COLS);
    if(getXPos()!=x) setXPos(x);
    if(getYPos()!=y) setYPos(y);
    setPos(TOPLEFT_XOFFSET + x * WorldView::TILE_SIZE, TOPLEFT_YOFFSET + y * WorldView::TILE_SIZE);
}

void MoveableSpriteItem::moveToDestination(SpriteItem& dest) {
    qDebug() << "move to new position";
    dest.updatePropertyView();
    path = findShortestPath(dest);
    emit strategyChanged(QString("State changed!"), path);
}

std::list<Tile> MoveableSpriteItem::findShortestPath(const SpriteItem& dest) const {
    if( &dest == this) return {};

    TileComparator comp(this, &dest);
    PriorityQueue<std::shared_ptr<Vertex>,TileComparator> openList(comp);
    std::list<std::shared_ptr<Vertex>> closedList;
    std::list<Tile> paths;
    openList.reserve(30);

    auto dummyVertex = parseToVertex();
    openList.push(dummyVertex);

    while(!openList.empty()) {
        auto top = openList.top();
        openList.pop();
        closedList.push_back(top);
        qDebug() << "Quequ size" << openList.size()
                 << "top:" << top->getXPos() << top->getYPos();

        auto edges = worldView->getNeighboringVertexs(*top);
        while(!edges.empty()){
            auto vertex = edges.last();
            edges.pop_back();

            int vX = vertex->getXPos();
            int vY = vertex->getYPos();

            float newCost = vertex->GCost + top->GCost;
            qDebug() << vX << vY << newCost;

            //the neighbor cannot refer back to the source vertex
//            if(vX == this->getXPos() && vY == this->getYPos()) {
//                qDebug() << "the neighbor cannot refer back to the sourc";
//                continue;
//            }

            //found: fill in the path by back-tracing and then return
            if( vX == dest.getXPos()&& vY == dest.getYPos()) {
                qDebug() << "\nTarget found";
                paths.push_back(Tile(vX,vY,newCost)); //store the accumulated GCost
                qDebug() << vX << vY << newCost ;
                Vertex* parent = vertex->parent;
                while(parent){
                    int x = parent->getXPos();
                    int y = parent->getYPos();
                    float v = parent->GCost;
                    paths.push_back(Tile(x,y,v));
                    qDebug() << x << y << v ;
                    parent = parent->parent;
                }
                paths.pop_back();
                return paths;
            }

            //avoid enemy in the path
            //must be placed after detecting found
            if(worldView->getEnemy(vX,vY)) newCost = 10000.0f;

            //lambda for checking whether the vertex already exists in an container
            auto exists = [vX,vY](const std::shared_ptr<Vertex>& v) ->bool {
                return vX == v->getXPos() && vY == v->getYPos();
            };

            //find in the open list to check whether already pushed into the priority queue";
            //if so, compare the G cost, and if the new G cost is smaller,
            //reset the cost and change its priority by reordering the priority queue
            auto open_it = std::find_if( openList.begin(), openList.end(), exists );

            //find in the closed list to check whether already visited as a parent vertex";
            //if so, compare the G cost, and if the new G cost is smaller, remove it back to the open list
            auto closed_it = std::find_if( closedList.begin(), closedList.end(), exists );

            if(open_it != openList.end()) {
                qDebug() << "found in the open list:"
                         << (*open_it)->getXPos() << (*open_it)->getYPos()
                         << "old GCost:" << (*open_it)->GCost << "new GCost:" << newCost;
                float oldCost = (*open_it)->GCost;
                if(newCost < oldCost) {
                    (*open_it)->GCost = newCost;
                    (*open_it)->parent = top.get();
                    openList.updatePriority();
                }
            } else if(closed_it != closedList.end()) {
                qDebug() << "found in the closed list: " << (*closed_it)->getXPos() << (*closed_it)->getYPos()
                         << "old GCost:" << (*closed_it)->GCost << "new GCost:" << newCost;
                float oldCost = (*closed_it)->GCost;
                if(newCost < oldCost) {
                    closedList.erase(closed_it);
                    vertex->GCost = newCost;
                    vertex->parent = top.get();
                    openList.push(vertex);
                }
            } else {
                vertex->GCost = newCost;
                openList.push(vertex);
            }
        }
    }

    /*
    int x = dest.getXPos(), y = dest.getYPos();
    auto destIt = std::find_if( closedList.begin(), closedList.end(),
                                [x,y](const std::shared_ptr<Vertex>& vertex) {
         return vertex->getXPos() == x && vertex->getYPos() == y;
    });
    float gcost = (*destIt)->GCost;
    paths.push_back(Tile(x,y,gcost)); //store the accumulated GCost
    Vertex* parent = (*destIt)->parent;
    while(parent){
        int x = parent->getXPos();
        int y = parent->getYPos();
        float v = parent->GCost;
        paths.push_back(Tile(x,y,v));
        qDebug() << x << y << v ;
        parent = parent->parent;
    }
    qDebug() << "total steps:" << steps << closedList.size();
    paths.pop_back(); */

    return paths;
}

float MoveableSpriteItem::getLowestCost(const SpriteItem& dest) const {
    std::list<Tile> path = findShortestPath(dest);
    if(path.empty()) return 10000.0f;
    Tile tile =  path.front();
    return tile.getValue()*WorldView::COST_FACTOR;
}

std::shared_ptr<Vertex> MoveableSpriteItem::parseToVertex() const {
    qDebug() << "MoveableSpriteItem::parseToVertex()";
    return std::make_shared<Vertex>(this->getXPos(),this->getYPos(),this->getValue());
}

bool TileComparator::operator() (const std::shared_ptr<Vertex>& a, const std::shared_ptr<Vertex>& b) const {
    assert(host != nullptr && dest != nullptr);

    int da = host->worldView->getTileDistance(*a,*dest);
    int db = host->worldView->getTileDistance(*b,*dest);

    float h = host->worldView->getheuristic();
    float FA = a->GCost + h * da;
    float FB = b->GCost + h * db;
    return FA > FB;  //in ascending order, the least element will be first poped
}



