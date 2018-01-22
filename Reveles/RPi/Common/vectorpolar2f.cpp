#include "vectorpolar2f.h"

VectorPolar2f::VectorPolar2f()
{

}

/*!
 * \brief VectorPolar2f::VectorPolar2f
 * \param r radius, length of vector
 * \param theta angle (in radians) CCW from 0 deg (East)
 */
VectorPolar2f::VectorPolar2f(float r, float theta)
{
    this->radius = r;
    this->theta = theta;
}
