#ifndef TILE_H
#define TILE_H

#include <QPainter>
#include "../RPi/Common/datatypes.h"
#include "noderenders.h"
#include <QPen>

class Tile
{
public:
    Tile();
    Tile(NodeType t);
    Tile(int x, int y, NodeType t);

    void drawTile(QPainter *painter);

    void SetXY(int x, int y);
    void SetDim(int w, int h);
    void SetNodeType(NodeType t);
    void SetRoute(bool r);
    void SetHasReveles(bool r);

    NodeType GetNodeType();

private:
    int gridX, gridY;
    int width, height;
    NodeType nt;
    bool isRoute;
    bool hasReveles;

};

#endif // TILE_H
