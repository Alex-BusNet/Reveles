#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include <QPainter>
#include "tile.h"
#include "../RPi/Common/datatypes.h"

class MapView : public QWidget
{
    Q_OBJECT
public:
    explicit MapView(QWidget *parent = nullptr);

    void setScreenSize(int w, int h);
    void paintEvent(QPaintEvent *event);

public slots:
    void SetPathInfo(QVector<GPSCoord> path);

private:
    QVector<Tile*> map;

    int revelesTile, targetTile, startTile;
    int screenSizeX, screenSizeY;
    int mapX, mapY;
    int tileWidth, tileHeight;

signals:

public slots:
};

#endif // MAPVIEW_H
