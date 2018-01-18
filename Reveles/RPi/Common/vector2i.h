#ifndef VECTOR2I_H
#define VECTOR2I_H

#include <tuple>

using namespace std;

class Vector2i
{
public:
    Vector2i();
    Vector2i(int i, int j);

    bool operator==(const Vector2i &other)
    {
        return ((this->i == other.i) && (this->j == other.j));
    }

    bool operator ==(const tuple<int,int> &pt)
    {
        return ((this->i == get<0>(pt)) && (this->j == get<1>(pt)));
    }

    Vector2i& operator-(const Vector2i &other)
    {
        return *(new Vector2i((this->i - other.i), this->j - other.j));
    }

    Vector2i& operator+(const Vector2i &other)
    {
        return *(new Vector2i((this->i + other.i), (this->j + other.j)));
    }

    Vector2i& operator=(const tuple<int, int> &pt)
    {
        return *(new Vector2i(get<0>(pt), get<1>(pt)));
    }

private:
    int i, j;

};

#endif // VECTOR2I_H
