#ifndef _VECTOR3_H
#define _VECTOR3_H

#include <cmath>
#include <string>
#include <sstream>
#include <vector>

namespace tbe
{

/// \brief Template 3D Vector

template<typename T> class Vector3
{
public:

    Vector3()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    template<typename T2> Vector3(const Vector3<T2>& vec)
    {
        x = static_cast<T2>(vec.x);
        y = static_cast<T2>(vec.y);
        z = static_cast<T2>(vec.z);
    }

    Vector3(const Vector3& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
    }

    Vector3(const T& value)
    {
        x = value;
        y = value;
        z = value;
    }

    Vector3(const T& fx, const T& fy, const T& fz)
    {
        x = fx;
        y = fy;
        z = fz;
    }

    Vector3(const std::string& exp)
    {
        char sep;
        std::istringstream str(exp);
        str >> x >> sep >> y >> sep >> z;
    }

    template<typename T2> Vector3 & operator=(const Vector3<T2>& vec)
    {
        x = static_cast<T2>(vec.x);
        y = static_cast<T2>(vec.y);
        z = static_cast<T2>(vec.z);

        return *this;
    }

    Vector3 & operator()(const Vector3& vec)
    {
        *this = vec;

        return *this;
    }

    Vector3 & operator()(const T& fx, const T& fy, const T& fz)
    {
        x = fx;
        y = fy;
        z = fz;

        return *this;
    }

    Vector3 & operator()(const T& value)
    {
        x = value;
        y = value;
        z = value;

        return *this;
    }

    T operator[](unsigned i) const
    {
        switch(i)
        {
        case 0: return x;
        case 1: return y;
        default: return z;
        }
    }

    Vector3 & operator+=(const Vector3& vect)
    {
        x += vect.x;
        y += vect.y;
        z += vect.z;

        return *this;
    }

    Vector3 & operator-=(const Vector3& vect)
    {
        x -= vect.x;
        y -= vect.y;
        z -= vect.z;

        return *this;
    }

    Vector3 & operator*=(const Vector3& vect)
    {
        x *= vect.x;
        y *= vect.y;
        z *= vect.z;

        return *this;
    }

    Vector3 & operator/=(const Vector3& vect)
    {
        x /= vect.x;
        y /= vect.y;
        z /= vect.z;

        return *this;
    }

    Vector3 & operator+=(const T& value)
    {
        x += value;
        y += value;
        z += value;

        return *this;
    }

    Vector3 & operator-=(const T& value)
    {
        x -= value;
        y -= value;
        z -= value;

        return *this;
    }

    Vector3 & operator*=(const T& value)
    {
        x *= value;
        y *= value;
        z *= value;

        return *this;
    }

    Vector3 & operator/=(const T& value)
    {
        x /= value;
        y /= value;
        z /= value;

        return *this;
    }

    Vector3 & operator=(const T& value)
    {
        x = value;
        y = value;
        z = value;

        return *this;
    }

    bool operator!() const
    {
        return (!x && !y && !z);
    }

    bool IsNull()
    {
        return (!x && !y && !z);
    }

    operator bool() const
    {
        return (x || y || z);
    }

    Vector3 operator-() const
    {
        return Vector3(*this) * (T) - 1;
    }

    operator T*()
    {
        return &x;
    }

    bool operator==(const Vector3& vect1) const
    {
        return (vect1.x == x && vect1.y == y && vect1.z == z);
    }

    bool operator!=(const Vector3& vect1) const
    {
        return !(vect1.x == x && vect1.y == y && vect1.z == z);
    }

    bool operator>(const Vector3& vect1) const
    {
        return (GetMagnitude() > vect1.GetMagnitude());
    }

    bool operator<(const Vector3& vect1) const
    {
        return (GetMagnitude() < vect1.GetMagnitude());
    }

    bool operator==(const T& value) const
    {
        return (x == value && y == value && z == value);
    }

    bool operator!=(const T& value) const
    {
        return !(x == value && y == value && z == value);
    }

    bool operator>(const T& value) const
    {
        return (GetMagnitude() > value);
    }

    bool operator<(const T& value) const
    {
        return (GetMagnitude() < value);
    }

    Vector3 operator+(const Vector3& vec) const
    {
        return Vector3(*this) += vec;
    }

    Vector3 operator-(const Vector3& vec) const
    {
        return Vector3(*this) -= vec;
    }

    Vector3 operator*(const Vector3& vec) const
    {
        return Vector3(*this) *= vec;
    }

    Vector3 operator/(const Vector3& vec) const
    {
        return Vector3(*this) /= vec;
    }

    Vector3 operator+(const T& value) const
    {
        return Vector3(*this) += value;
    }

    Vector3 operator-(const T& value) const
    {
        return Vector3(*this) -= value;
    }

    Vector3 operator*(const T& value) const
    {
        return Vector3(*this) *= value;
    }

    Vector3 operator/(const T& value) const
    {
        return Vector3(*this) /= value;
    }

    friend Vector3 operator+(const T& value, const Vector3& vec)
    {
        return Vector3(value) += vec;
    }

    friend Vector3 operator-(const T& value, const Vector3& vec)
    {
        return Vector3(value) -= vec;
    }

    friend Vector3 operator*(const T& value, const Vector3& vec)
    {
        return Vector3(value) *= vec;
    }

    friend Vector3 operator/(const T& value, const Vector3& vec)
    {
        return Vector3(value) /= vec;
    }

    Vector3& Rotate(const T& xrel, const T& yrel)
    {
        float m = GetMagnitude();

        z = cos(yrel * M_PI / 180) * cos(xrel * M_PI / 180);
        y = sin(yrel * M_PI / 180);
        x = cos(yrel * M_PI / 180) * sin(xrel * M_PI / 180);

        *this *= m;

        return *this;
    }

    static Vector3 Rotate(const Vector3& vec, const T& xrel, const T& yrel)
    {
        return Vector3(vec).Rotate(xrel, yrel);
    }

    Vector3& Normalize()
    {
        T length = GetMagnitude();

        return (length == 0) ? (*this)(0) : (*this /= length);
    }

    static Vector3 Normalize(const Vector3& vec)
    {
        return Vector3(vec).Normalize();
    }

    T GetMagnitude() const
    {
        return (T)sqrt(x * x + y * y + z * z);
    }

    float GetAverage() const
    {
        return (x + y + z) / 3;
    }

    bool IsInside(const Vector3& min, const Vector3& max) const
    {
        return (x >= min.x && y >= min.y && z >= min.z && x <= max.x && y <= max.y && z <= max.z);
    }

    static float Dot(const Vector3& a, const Vector3& b)
    {
        return (a.x * b.x + a.y * b.y + a.z * b.z);
    }

    static Vector3 Cross(const Vector3& a, const Vector3& b)
    {
        Vector3 n;

        n.x = (a.y * b.z) - (a.z * b.y);
        n.y = (a.z * b.x) - (a.x * b.z);
        n.z = (a.x * b.y) - (a.y * b.x);

        return n;
    }

    typedef std::vector< Vector3<T> > Array;

    T x, y, z;
};

typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;

}

#endif
