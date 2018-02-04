#ifndef VECTORPOLAR2F_H
#define VECTORPOLAR2F_H

#include <tuple>
#include <cmath>
#include "vector2f.h"

using namespace std;

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

    VectorPolar2f& operator=(const tuple<float, float> &pt)
    {
        return *(new VectorPolar2f(get<0>(pt), get<1>(pt)));
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
