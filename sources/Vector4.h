#ifndef _VECTOR4_H
#define _VECTOR4_H

#include <cmath>
#include <vector>
#include <sstream>

namespace tbe
{

/// \brief Template 4D Vector

template<typename T> class Vector4
{
public:

    Vector4()
    {
        x = 0;
        y = 0;
        z = 0;
        w = 0;
    }

    Vector4(const Vector4& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        w = vec.w;
    }

    template<typename T2> Vector4(const Vector4<T2>& vec)
    {
        x = static_cast<T2>(vec.x);
        y = static_cast<T2>(vec.y);
        z = static_cast<T2>(vec.z);
        w = static_cast<T2>(vec.w);
    }

    Vector4(T fx, T fy, T fz, T fw)
    {
        x = fx;
        y = fy;
        z = fz;
        w = fw;
    }

    Vector4(T value)
    {
        x = value;
        y = value;
        z = value;
        w = value;
    }

    template<typename T2> Vector4 & operator=(const Vector4<T2>& vec)
    {
        x = static_cast<T2>(vec.x);
        y = static_cast<T2>(vec.y);
        z = static_cast<T2>(vec.z);
        w = static_cast<T2>(vec.w);

        return *this;
    }

    Vector4 & operator=(const Vector4& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        w = vec.w;

        return *this;
    }

    Vector4 & operator()(const Vector4& vec)
    {
        *this = vec;

        return *this;
    }

    Vector4 & operator()(const T& fx, const T& fy, const T& fz, const T& fw)
    {
        x = fx;
        y = fy;
        z = fz;
        w = fw;

        return *this;
    }

    Vector4 & operator()(const T& value)
    {
        x = value;
        y = value;
        z = value;
        w = value;

        return *this;
    }

    T operator[](unsigned i)const
    {
        switch(i)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return w;
        }
    }

    Vector4 & operator+=(const Vector4& vect)
    {
        x += vect.x;
        y += vect.y;
        z += vect.z;
        w += vect.w;

        return *this;
    }

    Vector4 & operator-=(const Vector4& vect)
    {
        x -= vect.x;
        y -= vect.y;
        z -= vect.z;
        w -= vect.w;

        return *this;
    }

    Vector4 & operator*=(const Vector4& vect)
    {
        x *= vect.x;
        y *= vect.y;
        z *= vect.z;
        w *= vect.w;

        return *this;
    }

    Vector4 & operator/=(const Vector4& vect)
    {
        x /= vect.x;
        y /= vect.y;
        z /= vect.z;
        w /= vect.w;

        return *this;
    }

    Vector4 & operator+=(const T& value)
    {
        x += value;
        y += value;
        z += value;
        w += value;

        return *this;
    }

    Vector4 & operator-=(const T& value)
    {
        x -= value;
        y -= value;
        z -= value;
        w -= value;

        return *this;
    }

    Vector4 & operator*=(const T& value)
    {
        x *= value;
        y *= value;
        z *= value;
        w *= value;

        return *this;
    }

    Vector4 & operator/=(const T& value)
    {
        x /= value;
        y /= value;
        z /= value;
        w /= value;

        return *this;
    }

    Vector4 & operator=(const T& value)
    {
        x = value;
        y = value;
        z = value;
        w = value;

        return *this;
    }

    bool operator!() const
    {
        return (!x && !y && !z && !w);
    }

    Vector4 operator-() const
    {
        return Vector4(*this) * -1;
    }

    operator T*()
    {
        return &x;
    }

    bool operator==(const Vector4& vect1) const
    {
        return (vect1.x == x && vect1.y == y && vect1.z == z && vect1.w == w);
    }

    bool operator!=(const Vector4& vect1) const
    {
        return !(vect1.x == x && vect1.y == y && vect1.z == z && vect1.w == w);
    }

    bool operator>(const Vector4& vect1) const
    {
        return (getMagnitude() > vect1.getMagnitude());
    }

    bool operator<(const Vector4& vect1) const
    {
        return (getMagnitude() < vect1.getMagnitude());
    }

    bool operator==(const T& value) const
    {
        return (x == value && y == value && z == value && w == value);
    }

    bool operator!=(const T& value) const
    {
        return !(x == value && y == value && z == value && w == value);
    }

    bool operator>(const T& value) const
    {
        return (getMagnitude() > value);
    }

    bool operator<(const T& value) const
    {
        return (getMagnitude() < value);
    }

    Vector4 operator+(const Vector4& vect1) const
    {
        return Vector4(*this) += vect1;
    }

    Vector4 operator-(const Vector4& vect1) const
    {
        return Vector4(*this) -= vect1;
    }

    Vector4 operator*(const Vector4& vect1) const
    {
        return Vector4(*this) *= vect1;
    }

    Vector4 operator/(const Vector4& vect1) const
    {
        return Vector4(*this) /= vect1;
    }

    Vector4 operator+(const T& value) const
    {
        return Vector4(*this) += value;
    }

    Vector4 operator-(const T& value) const
    {
        return Vector4(value) -= value;
    }

    Vector4 operator*(const T& value) const
    {
        return Vector4(*this) *= value;
    }

    Vector4 operator/(const T& value) const
    {
        return Vector4(*this) /= value;
    }

    friend Vector4 operator+(const T& value, const Vector4& vec)
    {
        return Vector4(value) += vec;
    }

    friend Vector4 operator-(const T& value, const Vector4& vec)
    {
        return Vector4(value) -= vec;
    }

    friend Vector4 operator*(const T& value, const Vector4& vec)
    {
        return Vector4(value) *= vec;
    }

    friend Vector4 operator/(const T& value, const Vector4& vec)
    {
        return Vector4(value) /= vec;
    }

    Vector4& rotate(T xrel, T yrel)
    {
        float m = getMagnitude();

        x = cos(yrel * M_PI / 180) * cos(xrel * M_PI / 180) * getMagnitude();
        y = sin(yrel * M_PI / 180) * getMagnitude();
        z = cos(yrel * M_PI / 180) * sin(xrel * M_PI / 180) * getMagnitude();

        *this *= m;

        return *this;
    }

    static Vector4 rotate(const Vector4& vec, T xrel, T yrel)
    {
        return Vector4(vec).rotate(xrel, yrel);
    }

    Vector4& normalize()
    {
        T length = getMagnitude();

        if(length != 0)
        {
            x /= length;
            y /= length;
            z /= length;

            return *this;
        }

        else
            return (*this)(0);
    }

    Vector4& normalizeW()
    {
        T length = getMagnitude();

        return (length == 0) ? (*this)(0) : (*this /= length);
    }

    static Vector4 normalize(const Vector4& vec)
    {
        return Vector4(vec).normalize();
    }

    T getMagnitude() const
    {
        return (T)sqrt(x * x + y * y + z * z);
    }

    float getAverage() const
    {

        return (x + y + z + w) / 4;
    }

    bool isInside(const Vector4& min, const Vector4& max) const
    {

        return (x >= min.x && y >= min.y && z >= min.z && x <= max.x && y <= max.y && z <= max.z);
    }

    static Vector4 X(float value = 1)
    {
        return Vector4(value, 0, 0, 0);
    }

    static Vector4 Y(float value = 1)
    {
        return Vector4(0, value, 0, 0);
    }

    static Vector4 Z(float value = 1)
    {
        return Vector4(0, 0, value, 0);
    }

    static Vector4 W(float value = 1)
    {
        return Vector4(0, 0, 0, value);
    }

    static float dot(const Vector4& a, const Vector4& b)
    {
        return (a.x * b.x + a.y * b.y + a.z * b.z);
    }

    static Vector4 cross(const Vector4& a, const Vector4& b)
    {
        Vector4 n;

        n.x = (a.y * b.z) - (a.z * b.y);
        n.y = (a.z * b.x) - (a.x * b.z);
        n.z = (a.x * b.y) - (a.y * b.x);
        n.w = 0;

        return n;
    }

    friend std::istream & operator >>(std::istream& stream, Vector4<T>& vec)
    {
        char sep;
        return stream >> vec.x >> sep >> vec.y >> sep >> vec.z >> sep >> vec.w;
    }

    friend std::ostream & operator <<(std::ostream& stream, Vector4<T> vec)
    {
        return stream << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w;
    }

    typedef std::vector< Vector4<T> > Array;

    T x, y, z, w;
};

typedef Vector4<float> Vector4f;
typedef Vector4<int> Vector4i;

}

#endif
