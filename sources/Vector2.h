#ifndef VECTOR2_H
#define VECTOR2_H

#include <vector>
#include <cmath>

namespace tbe
{

/// \brief Template 2D Vector

template<typename T> class Vector2
{
public:

    Vector2()
    {
        x = 0;
        y = 0;
    }

    template<typename T2> Vector2(const Vector2<T2>& vec)
    {
        x = static_cast<T2>(vec.x);
        y = static_cast<T2>(vec.y);
    }

    Vector2(const Vector2& vec)
    {
        x = vec.x;
        y = vec.y;
    }

    Vector2(T fx, T fy)
    {
        x = fx;
        y = fy;
    }

    Vector2(T value)
    {
        x = value;
        y = value;
    }

    template<typename T2> Vector2 & operator=(const Vector2<T2>& vec)
    {
        x = static_cast<T2>(vec.x);
        y = static_cast<T2>(vec.y);

        return *this;
    }

    Vector2 & operator()(const Vector2& vec)
    {
        *this = vec;

        return *this;
    }

    Vector2 & operator()(const T& fx, const T& fy)
    {
        x = fx;
        y = fy;

        return *this;
    }

    Vector2 & operator()(const T& value)
    {
        x = value;
        y = value;

        return *this;
    }

    T operator[](unsigned i)const
    {
        switch(i)
        {
            case 0: return x;
            default: return y;
        }
    }

    Vector2 & operator+=(const Vector2& vect)
    {
        x += vect.x;
        y += vect.y;

        return *this;
    }

    Vector2 & operator-=(const Vector2& vect)
    {
        x -= vect.x;
        y -= vect.y;

        return *this;
    }

    Vector2 & operator*=(const Vector2& vect)
    {
        x *= vect.x;
        y *= vect.y;

        return *this;
    }

    Vector2 & operator/=(const Vector2& vect)
    {
        x /= vect.x;
        y /= vect.y;

        return *this;
    }

    Vector2 & operator+=(const T& value)
    {
        x += value;
        y += value;

        return *this;
    }

    Vector2 & operator-=(const T& value)
    {
        x -= value;
        y -= value;

        return *this;
    }

    Vector2 & operator*=(const T& value)
    {
        x *= value;
        y *= value;

        return *this;
    }

    Vector2 & operator/=(const T& value)
    {
        x /= value;
        y /= value;

        return *this;
    }

    Vector2 & operator=(const T& value)
    {
        x = value;
        y = value;

        return *this;
    }

    bool operator!() const
    {
        return (!x && !y);
    }

    Vector2 operator-() const
    {
        return (*this) * (T) - 1;
    }

    operator T*()
    {
        return &x;
    }

    bool operator==(const Vector2& vect1) const
    {
        return (x == vect1.x && y == vect1.y);
    }

    bool operator!=(const Vector2& vect1) const
    {
        return !(x == vect1.x && y == vect1.y);
    }

    bool operator>(const Vector2& vect1) const
    {
        return (GetMagnitude() > vect1.GetMagnitude());
    }

    bool operator<(const Vector2& vect1) const
    {
        return (GetMagnitude() < vect1.GetMagnitude());
    }

    bool operator==(const T& value) const
    {
        return (x == value && y == value);
    }

    bool operator!=(const T& value) const
    {
        return !(x == value && y == value);
    }

    bool operator>(const T& value) const
    {
        return (GetMagnitude() > value);
    }

    bool operator<(const T& value) const
    {
        return (GetMagnitude() < value);
    }

    Vector2 operator+(const Vector2& vect1) const
    {
        return Vector2(*this) += vect1;
    }

    Vector2 operator-(const Vector2& vect1) const
    {
        return Vector2(*this) -= vect1;
    }

    Vector2 operator*(const Vector2& vect1) const
    {
        return Vector2(*this) *= vect1;
    }

    Vector2 operator/(const Vector2& vect1) const
    {
        return Vector2(*this) /= vect1;
    }

    Vector2 operator+(const T& value) const
    {
        return Vector2(*this) += value;
    }

    Vector2 operator-(const T& value) const
    {
        return Vector2(*this) -= value;
    }

    Vector2 operator*(const T& value) const
    {
        return Vector2(*this) *= value;
    }

    Vector2 operator/(const T& value) const
    {
        return Vector2(*this) /= value;
    }

    friend Vector2 operator+(const T& value, const Vector2& vec)
    {
        return Vector2(value) += vec;
    }

    friend Vector2 operator-(const T& value, const Vector2& vec)
    {
        return Vector2(value) -= vec;
    }

    friend Vector2 operator*(const T& value, const Vector2& vec)
    {
        return Vector2(value) *= vec;
    }

    friend Vector2 operator/(const T& value, const Vector2& vec)
    {
        return Vector2(value) /= vec;
    }

    Vector2& Rotate(T xrel, T yrel)
    {
        x = cos(yrel * M_PI / 180) * cos(xrel * M_PI / 180) * GetMagnitude();
        y = sin(yrel * M_PI / 180) * GetMagnitude();

        return *this;
    }

    static Vector2 Rotate(const Vector2& vec, T xrel, T yrel)
    {
        return Vector2(vec).Rotate(xrel, yrel);
    }

    Vector2& Normalize()
    {
        T length = GetMagnitude();

        return (length == 0) ? (*this)(0) : (*this /= length);
    }

    static Vector2 GetNormalize(const Vector2& vec)
    {
        return Vector2(vec).Normalize();
    }

    T GetMagnitude() const
    {
        return (T)sqrt(x * x + y * y);
    }

    bool IsInsinde(Vector2 pos, Vector2 size)
    {
        return (x >= pos.x && x <= pos.x + size.x &&
                y >= pos.y && y <= pos.y + size.y);
    }

    float GetAverage() const
    {
        return (x + y) / 2;
    }

    static float Dot(const Vector2& a, const Vector2& b)
    {
        return (a.x * b.x + a.y * b.y);
    }

    static Vector2 X(float value = 1)
    {
        return Vector2(value, 0);
    }

    static Vector2 Y(float value = 1)
    {
        return Vector2(0, value);
    }

    T x, y;

    typedef std::vector< Vector2<T> > Array;
};

typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;

}

#endif
