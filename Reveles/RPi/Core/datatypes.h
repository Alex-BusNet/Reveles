#ifndef DATATYPES_H
#define DATATYPES_H

struct DecisionPoint {
    MagDirection face;
    GPSCoord point;
};

struct GPSCoord {
    float latitude;
    float longitude;

    bool operator==(GPSCoord& other)
    {
        if((this->latitude == other.latitude)
                && (this->longitude == other.longitude))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

struct MagDirection {
    float x;
    float y;
    float z;
};

struct Path {
    // NOOP
};

struct Node {
    // NOOP
};

enum Direction {
    N, NE, NW,
    S, SE, SW,
    W, WN, WS,
    E, EN, ES
};

#endif // DATATYPES_H
