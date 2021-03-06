#include "revelesmap.h"

Q_GLOBAL_STATIC(RevelesMap, rm)

RevelesMap::RevelesMap()
{

}

RevelesMap *RevelesMap::instance()
{
    return rm;
}

void RevelesMap::Init()
{
    instance()->setObjectName("RevelesMap");
    offsetLat = 41.631906;
    offsetLong = -85.006118;

    // Grid size: 120 tile x 63 tile (approx)
    for(int x = 0; x < 120; x++)
    {
        for(int y = 0; y < 63; y++)
        {
            MapNode *n = new MapNode();
            n->coord = GPSCoord
                        {
                            // Need to check accuracy of these values
                            offsetLat +  ((THRESHOLD_LATITUDE * 2) * (x + 1)),
                            offsetLong + ((THRESHOLD_LONGITUDE * 2) * (y +1))
                        };
            n->nt = UNKNOWN;
            n->mapX = x;
            n->mapY = y;

            grid.push_back(n);
        }
    }
}

GPSCoord RevelesMap::GetOffset()
{
    return GPSCoord{offsetLat, offsetLong};
}

double RevelesMap::GetLatitudeThreshold()
{
    return THRESHOLD_LATITUDE;
}

double RevelesMap::GetLongitudeThreshold()
{
    return THRESHOLD_LONGITUDE;
}

MapNode* RevelesMap::GetNodeFromCoord(const GPSCoord &gpsc)
{
    double tileX, tileY;

    Logger::writeLine(instance(), QString("%1, %2").arg(gpsc.latitude, 12, 'g', 10).arg(gpsc.longitude, 12, 'g', 10));

    tileX = ((gpsc.longitude - offsetLong) * std::pow(10,5) );
    tileY = ((gpsc.latitude - offsetLat) * std::pow(10,5) );

    Logger::writeLine(instance(), QString("%1, %2").arg(tileX).arg(tileY));

    tileX /= 120;
    tileY /= 63;

    int idx = tileX + (120 * tileY);

    if (idx > grid.size())
    {
        Logger::writeLine(instance(), Reveles::IOR);
        return NULL;
    }

    return grid.at(idx);
}

MapNode *RevelesMap::GetNodeFromPoint(int x, int y)
{
    return grid.at(x + (120 * y));
}

/**
 * @brief Updates the NodeType of the selected node
 * @param x The x coordinate of the tile
 * @param y The y corrdinate of the tile
 * @param t The new NodeType of the tile
 */
void RevelesMap::UpdatePath(int x, int y, NodeType t)
{
    Logger::writeLine(instance(), QString("Map Node updated"));
    // Need to double check this calculation
    grid.at(x + (120 * y))->nt = t;
}

void RevelesMap::AddPoint(GPSCoord gpsc)
{
    // One grid square corresponds to a 2ft x 2ft area.

//    if(!gridHasPoint(gpsc))
//        grid.push_back(gpsc);
}

list<GPSCoord> RevelesMap::GetNearbyPoints(GPSCoord gpsc)
{
    list<GPSCoord> neighbors;

    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            if(x == y) { continue; }

            int checkX;
            int checkY;
        }
    }

    return neighbors;
}

void RevelesMap::RegisterDestination(string name, GPSCoord gpsc)
{

}

///*
// * FindPaths(string) assumes that the destination
// * passed to it exists.
// */
//list<Node*> RevelesMap::FindPath(string dest)
//{
//    return FindPath(&savedDestinations[dest]);
//}

void RevelesMap::saveMapData()
{
    if(!QDir("Assets/SaveData").exists())
        QDir().mkdir("Assets/SaveData");

    QFile rmSaveData("Assets/SaveData/revelesMapData.json");
    if(!rmSaveData.open(QIODevice::WriteOnly))
    {
        Logger::writeLine(instance(), Reveles::MAP_SAVE_ERR);
    }
    else
    {
        Logger::writeLine(instance(), Reveles::MAP_SAVE_PROG);
        QJsonDocument doc;
        QJsonArray mdArr;

        foreach(MapNode *n, grid)
        {
            QJsonObject obj;
            obj["latitude"] = n->coord.latitude;
            obj["longitude"] = n->coord.longitude;
            obj["nodetype"] = n->nt;
            obj["mapx"] = n->mapX;
            obj["mapy"] = n->mapY;
            mdArr.push_back(obj);
        }

        doc.setArray(mdArr);
        rmSaveData.write(doc.toJson());

        rmSaveData.flush();

        Logger::writeLine(instance(), "Save complete");
    }

    if(rmSaveData.isOpen())
        rmSaveData.close();
}

void RevelesMap::LoadMapData()
{
    QFile rmSaveData("Assets/SaveData/revelesMapData.json");
    if(!rmSaveData.open(QIODevice::ReadOnly))
    {
        Logger::writeLine(instance(), Reveles::MAP_LOAD_ERR);
    }
    else
    {
        Logger::writeLine(instance(), Reveles::MAP_LOAD_PROG);
        QJsonDocument doc = QJsonDocument::fromJson(rmSaveData.readAll());
        QJsonArray mdArr = doc.array();

        for(int i = 0; i < mdArr.size(); i++)
        {
            QJsonObject obj = mdArr[i].toObject();
            MapNode *n;

            n->coord = GPSCoord {
                    obj["latitude"].toDouble(),
                    obj["longitude"].toDouble()
                };

            n->nt = (NodeType)obj["nodetype"].toInt();
            n->mapX = obj["mapx"].toInt();
            n->mapY = obj["mapy"].toInt();

            grid.push_back(n);

        }

        Logger::writeLine(instance(), "Load complete");
    }

    if(rmSaveData.isOpen())
        rmSaveData.close();
}

bool RevelesMap::gridHasPoint(GPSCoord gpsc)
{
    int tileX, tileY;

    tileX = (gpsc.longitude - offsetLong) / 120;
    tileY = (gpsc.latitude - offsetLat) / 63;

    double targetLat = grid.at(tileX + (120 * tileY))->coord.latitude;
    double targetLong = grid.at(tileX + (120 * tileY))->coord.longitude;

    if((gpsc.latitude > (targetLat - THRESHOLD_LATITUDE))
            && (gpsc.latitude < (targetLat + THRESHOLD_LATITUDE)))
    {
        if((gpsc.longitude > (targetLong - THRESHOLD_LONGITUDE))
                && (gpsc.longitude < (targetLong + THRESHOLD_LONGITUDE)))
            return true;
        else
            return false;
    }

    return false;
}
