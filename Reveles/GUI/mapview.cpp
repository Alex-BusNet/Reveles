#include "mapview.h"

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
}

void MapView::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);

    if(paint.isActive())
    {
        QPen *p = new QPen(QColor("#5ac5cc"));
        p->setWidthF(0.5);
        paint.setPen(*p);

        for(int x = 0; x < 120; x++)
        {
            for(int y = 0; y < 63; y++)
            {
                paint.drawRect(x + (tileWidth * x), y + (tileHeight * y),
                                tileWidth, tileHeight);
            }
        }
    }
}
