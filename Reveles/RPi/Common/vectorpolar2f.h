#ifndef VECTORPOLAR2F_H
#define VECTORPOLAR2F_H

#include <tuple>
#include <cmath>
#include "vector2f.h"

using namespace std;

class Vector2f;

class VectorPolar2f
{
public:
    VectorPolar2f();
    VectorPolar2f(float r, float theta);

    bool operator==(const VectorPolar2f &other)
    {
        return ((this->radius == other.radius) && (this->theta == other.theta));
    }

    bool operator ==(const tuple<float, float> &pt)
    {
        return ((this->radius == get<0>(pt)) && (this->theta == get<1>(pt)));
    }

//    VectorPolar2f& operator-(const VectorPolar2f &other)
//    {
//        return (this->toCartesian() - other.toCartesian()).toPolar();
//    }

//    VectorPolar2f& operator+(const VectorPolar2f &other)
//    {
//        // Kind of a hacky way to do this, but I don't want to write the
//        // actual calculation right now. -Alex 1/21/18
//        return *((toCartesian() + other.toCartesian()).toPolar());
//    }

    VectorPolar2f& operator=(const tuple<float, float> &pt)
    {
        return *(new VectorPolar2f(get<0>(pt), get<1>(pt)));
    }

    Vector2f& toCartesian()
    {
        return *(new Vector2f(this->radius * cos(this->theta), this->radius * sin(this->theta)));
    }

    float length() { return radius; }
    /*!
     * \brief angle
     * \return angle (in radians) CCW from 0 deg (East)
     */
    float angle() { return theta; }

    /*!
     * \brief degrees
     * \return angle (in degrees) CCW from 0 deg (East)
     */
    float degrees() { return theta * (180 / M_PI); }

private:
    float radius, theta;
};

#endif // VECTORPOLAR2F_H
