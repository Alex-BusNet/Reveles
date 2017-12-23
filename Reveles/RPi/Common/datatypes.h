#ifndef DATATYPES_H
#define DATATYPES_H

struct GPSCoord {
    float latitude;
    float longitude;

    bool operator==(GPSCoord& other)
    {
        return ((this->latitude == other.latitude)
               && (this->longitude == other.longitude));
    }
};

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

enum UltrasonicPos {
    FORWARD, REAR, LEFT, RIGHT, STAIR
};

enum Mode { IN, OUT };

enum SensorType {US, PIR, TOF};



#endif // DATATYPES_H
