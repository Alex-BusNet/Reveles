#ifndef VECTOR2F_H
#define VECTOR2F_H

#include <tuple>
//#include "vectorpolar2f.h"
#include <cmath>

using namespace std;

//class VectorPolar2f;

class Vector2f
{
public:
    Vector2f();
    Vector2f(float i, float j);

    bool operator==(const Vector2f &other)
    {
        return ((this->i == other.i) && (this->j == other.j));
    }

    bool operator ==(const tuple<float,float> &pt)
    {
        return ((this->i == get<0>(pt)) && (this->j == get<1>(pt)));
    }

    Vector2f& operator-(const Vector2f &other)
    {
        return *(new Vector2f((this->i - other.i), this->j - other.j));
    }

    Vector2f& operator+(const Vector2f &other)
    {
        return *(new Vector2f((this->i + other.i), (this->j + other.j)));
    }

    Vector2f& operator=(const tuple<float, float> &pt)
    {
        return *(new Vector2f(get<0>(pt), get<1>(pt)));
    }

//    VectorPolar2f& toPolar()
//    {
//        return *(new VectorPolar2f(
//                     sqrt(pow(this->i, 2) + pow(this->j, 2)),
//                     tan(j / i)));
//    }

    float x() { return i; }
    float y() { return j; }

private:
    float i, j;

};

#endif // VECTOR2F_H
