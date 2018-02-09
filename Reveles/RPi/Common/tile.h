#ifndef TILE_H
#define TILE_H

#include "datatypes.h"

class Tile
{
public:
    Tile();
    Tile(GPSCoord gpsc);

    void setXY(int x, int y);

private:
    int gridX, gridY;
    GPSCoord center;
};

#endif // TILE_H
