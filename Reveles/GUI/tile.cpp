#include "tile.h"

Tile::Tile()
{
    gridX = 0;
    gridY = 0;
    width = 3;
    height = 3;
    nt = UNKNOWN;
}

Tile::Tile(NodeType t)
{
    this->nt = t;
}


Tile::Tile(int x, int y, NodeType t)
{
    this->gridX = x;
    this->gridY = y;
    this->nt = t;
    width = 3;
    height = 3;
    hasReveles = false;
    isRoute = false;
}

void Tile::drawTile(QPainter *painter)
{
    QPen *p = new QPen(Qt::white);
    QPen old = painter->pen();

    p->setWidthF(0.5);
    painter->setPen(*p);

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(nt == PATH && isRoute)
            {
                painter->fillRect(gridX, gridY, width, height, QColor("#ff8d00"));
            }
            else if(hasReveles)
            {
                painter->setBrush(QColor("#15ff00"));
                painter->drawEllipse(gridX, gridY, width, height);
                painter->setBrush(Qt::transparent);
            }
            else if(NodeRenders[nt][i][j] == 1)
            {
                QRectF *r = new QRectF(gridX + (j * (width / 3.0f)),
                        gridY + (i * (height / 3.0f)),
                        (width / 3.0f), (height / 3.0f));

                painter->fillRect(*r, Qt::white);
            }

        }
    }

    painter->setPen(old);

    painter->drawRect(gridX, gridY, width, height);

    delete p;
}

void Tile::SetXY(int x, int y)
{
    this->gridX = x;
    this->gridY = y;
}

void Tile::SetDim(int w, int h)
{
    width = w;
    height = h;
}

void Tile::SetNodeType(NodeType t)
{
    this->nt = t;
}

void Tile::SetRoute(bool r)
{
    this->isRoute = r;
}

void Tile::SetHasReveles(bool r)
{
    this->hasReveles = r;
}

NodeType Tile::GetNodeType()
{
    return nt;
}
