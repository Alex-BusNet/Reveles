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

enum Direction {
    N, NE, NW,
    S, SE, SW,
    W, WN, WS,
    E, EN, ES
};

#endif // DATATYPES_H
