#ifndef SFML_EXTRA_VECTOR_UTILS_H
#define SFML_EXTRA_VECTOR_UTILS_H

/**
    SFML is missing a few useful operators on the 2D vectors.
    These are the missing operators.
  */

namespace sf
{
    /** < and > operators are length compares. Return true or false if the distance is longer/shorter then asked distance. */
    template <typename T>
    static inline bool operator > (const Vector2<T>& v, const T& f)
    {
        return (v.x * v.x + v.y * v.y) > f * f;
    }

    template <typename T>
    static inline bool operator < (const Vector2<T>& v, const T& f)
    {
        return (v.x * v.x + v.y * v.y) < f * f;
    }

    template <typename T>
    static inline bool operator >= (const Vector2<T>& v, const T& f)
    {
        return (v.x * v.x + v.y * v.y) >= f * f;
    }

    template <typename T>
    static inline bool operator <= (const Vector2<T>& v, const T& f)
    {
        return (v.x * v.x + v.y * v.y) <= f * f;
    }
    
    template <typename T>
    Vector2<T> vector2FromAngle(const T& angle)
    {
        return Vector2<T>(sinf(angle / 180.0 * M_PI), -cosf(angle / 180.0 * M_PI));
    }

    template <typename T>
    T vector2ToAngle(const Vector2<T>& v)
    {
        return atan2(-v.x, v.y) / M_PI * 180;
    }
    
    template <typename T>
    T length(const Vector2<T>& v)
    {
        return sqrtf(v.x*v.x+v.y*v.y);
    }
    
    template <typename T>
    Vector2<T> normalize(const Vector2<T>& v)
    {
        return v / length(v);
    }
}

#endif//SFML_EXTRA_VECTOR_UTILS_H
