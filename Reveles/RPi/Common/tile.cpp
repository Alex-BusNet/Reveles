#include "tile.h"

Tile::Tile()
{

}

Tile::Tile(GPSCoord gpsc)
{
    this->center = gpsc;
}

void Tile::setXY(int x, int y)
{
    this->gridX = x;
    this->gridY = y;
}
