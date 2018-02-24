#include "mapview.h"
#include <random>
#include <ctime>

MapView::MapView(QWidget *parent) : QWidget(parent)
{
    mapX = 120;
    mapY = 63;
}

void MapView::setScreenSize(int w, int h)
{
    this->screenSizeX = w;
    this->screenSizeY = h;

    this->tileWidth = w / mapX;
    this->tileHeight = h / mapY;

    srand(time(0));

    int posX = 0, posY = 0;

    for(int x = 0; x < 120; x++)
    {
        for(int y = 0; y < 63; y++)
        {
            map.push_back(new Tile(posX, posY, UNKNOWN));
            map.last()->SetDim(tileWidth, tileHeight);

            posY += tileHeight;
        }

        posX += tileWidth;
        posY = 0;
    }
}

void MapView::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);

    if(paint.isActive())
    {
        QPen *p = new QPen(QColor("#5ac5cc"));
        p->setWidthF(0.5);
        paint.setPen(*p);

        foreach(Tile* t, map)
        {
            t->drawTile(&paint);
        }
    }
}

// The acutal contents of the QVector are subject to change.
// May turn into a list of integer values where each integer
//    is the index of the tile in the map object.
void MapView::SetPathInfo(QVector<GPSCoord> path)
{
//    foreach(GPSCoord t, path)
//    {
//        map[]->SetRoute(true);
//    }

//    map[]->SetHasReveles(true);
}
