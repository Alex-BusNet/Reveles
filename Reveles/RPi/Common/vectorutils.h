#ifndef VECTORUTILS_H
#define VECTORUTILS_H
#include "vector2f.h"
#include "vector2i.h"
#include "vectorpolar2f.h"
#include <cmath>

static VectorPolar2f& toPolar(Vector2f &v2f)
{
    return *(new VectorPolar2f(
                sqrt(pow(v2f.x(), 2) + pow(v2f.x(), 2)),
                tan(v2f.y() / v2f.y())
                ));
}

static Vector2f& toCartesian(VectorPolar2f &vp2f)
{
    return *(new Vector2f(vp2f.length() * cos(vp2f.angle()),
                    vp2f.length() * sin(vp2f.angle())));
}

/**
 * @brief Adds two polar vectors. Result is returned in first vector
 * @param First vector to add. Result saved here.
 * @param Second vector to add.
 */
static VectorPolar2f& AddPolar(VectorPolar2f v1, VectorPolar2f v2)
{
    // Kind of a hacky way to do this, but I don't want to write the
    // actual calculation right now. -Alex 2/4/18
    return toPolar((toCartesian(v1) + toCartesian(v2)));
}

/**
 * @brief Subtracts two polar vectors. Result is returned in first vector
 * @param First vector to subtract. Result saved here.
 * @param Second vector to stubtract.
 */
static VectorPolar2f& SubtractPolar(VectorPolar2f v1, VectorPolar2f v2)
{
    // Kind of a hacky way to do this, but I don't want to write the
    // actual calculation right now. -Alex 2/4/18
    return toPolar((toCartesian(v1) - toCartesian(v2)));
}
#endif // VECTORUTILS_H
