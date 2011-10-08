#ifndef _VECTOR3_H
#define _VECTOR3_H

#include <cmath>
#include <vector>
#include <sstream>

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

    T operator[](unsigned i)const
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
        return (getMagnitude() > vect1.getMagnitude());
    }

    bool operator<(const Vector3& vect1) const
    {
        return (getMagnitude() < vect1.getMagnitude());
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
        return (getMagnitude() > value);
    }

    bool operator<(const T& value) const
    {
        return (getMagnitude() < value);
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

    Vector3& rotate(const T& xrel, const T& yrel)
    {
        float m = getMagnitude();

        z = cos(yrel * M_PI / 180) * cos(xrel * M_PI / 180);
        y = sin(yrel * M_PI / 180);
        x = cos(yrel * M_PI / 180) * sin(xrel * M_PI / 180);

        *this *= m;

        return *this;
    }

    static Vector3 rotate(const Vector3& vec, const T& xrel, const T& yrel)
    {
        return Vector3(vec).rotate(xrel, yrel);
    }

    Vector3& normalize()
    {
        T length = getMagnitude();

        return (length == 0) ? (*this)(0) : (*this /= length);
    }

    static Vector3 normalize(const Vector3& vec)
    {
        return Vector3(vec).normalize();
    }

    T getMagnitude() const
    {
        return (T)sqrt(x * x + y * y + z * z);
    }

    float getAverage() const
    {
        return (x + y + z) / 3;
    }

    bool isInside(const Vector3& min, const Vector3& max) const
    {
        return (x >= min.x && y >= min.y && z >= min.z && x <= max.x && y <= max.y && z <= max.z);
    }

    void reSet(const Vector3<T>& diri)
    {
        x = !x ? 0 : (x > 0 ? diri.x : -diri.x);
        y = !y ? 0 : (y > 0 ? diri.y : -diri.y);
        z = !z ? 0 : (z > 0 ? diri.z : -diri.z);
    }

    Vector3& pinpoint()
    {
        if(x > 0.6666)
            return X(1).Y(0).Z(0);

        else if(x < -0.6666)
            return X(-1).Y(0).Z(0);

        else if(y > 0.6666)
            return X(0).Y(1).Z(0);

        else if(y < -0.6666)
            return X(0).Y(-1).Z(0);

        else if(z > 0.6666)
            return X(0).Y(0).Z(1);

        else if(z < -0.6666)
            return X(0).Y(0).Z(-1);

        else
            return *this;
    }

    Vector3 X()
    {
        return Vector3(x, 0, 0);
    }

    Vector3 Y()
    {
        return Vector3(0, y, 0);
    }

    Vector3 Z()
    {
        return Vector3(0, 0, z);
    }

    Vector3& X(float value)
    {
        x = value;
        return *this;
    }

    Vector3& Y(float value)
    {
        y = value;
        return *this;
    }

    Vector3& Z(float value)
    {
        z = value;
        return *this;
    }

    static float dot(const Vector3& a, const Vector3& b)
    {
        return (a.x * b.x + a.y * b.y + a.z * b.z);
    }

    static Vector3 cross(const Vector3& a, const Vector3& b)
    {
        Vector3 n;

        n.x = (a.y * b.z) - (a.z * b.y);
        n.y = (a.z * b.x) - (a.x * b.z);
        n.z = (a.x * b.y) - (a.y * b.x);

        return n;
    }

    friend std::istream & operator >>(std::istream& stream, tbe::Vector3<T>& vec)
    {
        char sep;
        return stream >> vec.x >> sep >> vec.y >> sep >> vec.z;
    }

    friend std::ostream & operator <<(std::ostream& stream, tbe::Vector3<T> vec)
    {
        return stream << vec.x << ", " << vec.y << ", " << vec.z;
    }

    /**
     * Interprétation d'une chaine de caracteres en vecteur a 3 composant
     *  depuis le format "x, y, z"
     */
    std::string toStr(char sep = ',') const
    {
        std::stringstream stream;
        stream << x << sep << y << sep << z;

        return stream.str();
    }

    /**
     * Conversion d'un vecteur a 3 composant en chaine de caracteres
     *  au format "x, y, z"
     */
    Vector3& fromStr(std::string str, bool withsep = true)
    {
        Vector3<T> vec;
        std::stringstream ss(str);

        if(withsep)
        {
            char sep;
            ss >> x >> sep >> y >> sep >> z;
        }
        else
        {
            ss >> x >> y >> z;
        }

        return *this;
    }

    typedef std::vector< Vector3<T> > Array;

    T x, y, z;
};

typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;

}

#endif
