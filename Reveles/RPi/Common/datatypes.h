#ifndef DATATYPES_H
#define DATATYPES_H
#include <QMetaType>
#include <QtDBus/QDBusArgument>
#include <cstring>

struct GPSCoord {
    double latitude;
    double longitude;

    bool operator==(GPSCoord& other)
    {
        return ((this->latitude == other.latitude)
               && (this->longitude == other.longitude));
    }

    GPSCoord operator+(std::tuple<double, double> &td)
    {
        return GPSCoord{this->latitude + std::get<0>(td), this->longitude + std::get<1>(td)};
    }

    GPSCoord operator-(std::tuple<double, double> &td)
    {
        return GPSCoord{this->latitude - std::get<0>(td), this->longitude - std::get<1>(td)};
    }
};

Q_DECLARE_METATYPE(GPSCoord)

//====================================================
// D-Bus Custom-type marshall and demarshall operator overloads.
QT_BEGIN_NAMESPACE

inline QDBusArgument &operator<<(QDBusArgument &argument, const GPSCoord& coord)
{
    argument.beginStructure();
    argument << coord.latitude;
    argument << coord.longitude;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, GPSCoord &coord)
{
    argument.beginStructure();
    argument >> coord.latitude;
    argument >> coord.longitude;
    argument.endStructure();
    return argument;
}

QT_END_NAMESPACE
//====================================================

struct AccelDirection { float x, y, z; };

struct MagDirection { float x, y, z; };

struct GyroDirection { float x, y, z; };

struct DecisionPoint {
    MagDirection face;
    GPSCoord point;
};

// The DOOR type may not be used.
enum NodeType { WALL, PATH, STAIRS, DOOR, UNKNOWN };

struct Node {
    GPSCoord coord;
    NodeType nt;
};

// This struct may be removed.
struct Path {
    GPSCoord coord;
    Node *child;
    Node *parent;
};

enum Direction {
    N, NE, NW,
    S, SE, SW,
    W, WN, WS,
    E, EN, ES
};

// Mounting location of HC-SR04 on Reveles.
enum UltrasonicPos {
    FORWARD, REAR, LEFT, RIGHT, STAIR
};

// Used in RevelesAnalyticalEngine::Navigate()
enum TravelMode { BLIND, WANDER };

enum SensorType {US, PIR, TOF};

// ActionState represents the detected
// and predicted actions an moving object
// would take.
enum ActionState
{
    L2R      = 0,// State 1
    R2L      = 1,// State 2
    TOWARDS  = 2,// State 3
    AWAY     = 3,// State 4
    DIAG_L2R = 4,// State 5
    DIAG_R2L = 5,// State 6
    STOP     = 6,// State 7
    NO_STATE = 7 // Initialization State.
};

static std::string DirString[] =
{
    "North", "North-East", "North-West",
    "South", "South-East", "South-West",
    "West", "West-North", "West-South",
    "East", "East-North", "East-South"
};

struct ObjectTracking
{
    int index;
    ActionState dir;
    int zone;
};

enum FutureStatus {STOPPED, RUNNING, PAUSED, FINISHED, NON_EXISTENT };

#endif // DATATYPES_H
