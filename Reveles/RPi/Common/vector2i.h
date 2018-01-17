#ifndef VECTOR2I_H
#define VECTOR2I_H


class Vector2i
{
public:
    Vector2i();
    Vector2i(int i, int j);

    bool operator==(const Vector2i &other)
    {
        return ((this->i == other.i) && (this->j == other.j));
    }

    Vector2i& operator-(const Vector2i &other)
    {
        return new Vector2i(this->i - other.i, this->j - other.j);
    }

    Vector2i& operator+(const Vector2i &other)
    {
        return new Vector2i(this->i + other.i, this->j + other.j);
    }

private:
    int i, j;

};

#endif // VECTOR2I_H
