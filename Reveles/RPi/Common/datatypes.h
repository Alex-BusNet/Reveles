#ifndef DATATYPES_H
#define DATATYPES_H
#include <QMetaType>
#include <QtDBus/QDBusArgument>

struct GPSCoord {
    double latitude;
    double longitude;

    bool operator==(GPSCoord& other)
    {
        return ((this->latitude == other.latitude)
               && (this->longitude == other.longitude));
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

struct MagDirection {
    float x;
    float y;
    float z;
};

struct DecisionPoint {
    MagDirection face;
    GPSCoord point;
};

struct Path {
    // NOOP
};

struct Node {
    GPSCoord coord;
    Node *child;
    Node *parent;
    // NOOP
};

struct Accel { float x, y, z; };
struct Gyro  { float x, y, z; };
struct Mag   { float x, y, z; };

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

// GPIO Pin mode.
// Most likely will be deprecated.
enum Mode { IN, OUT };

enum SensorType {US, PIR, TOF};



#endif // DATATYPES_H
