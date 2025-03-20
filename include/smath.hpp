#ifndef SMATH_HPP
#define SMATH_HPP

#include <cmath>

constexpr double PI = 3.14159265358979323846;
constexpr double EPSILON = 1e-6;
constexpr double DEG2RAD = PI / 180.0;
constexpr double RAD2DEG = 180.0 / PI;

class Vector3 {
public:
    double x, y, z;
    
    constexpr Vector3(double x_ = 0.0, double y_ = 0.0, double z_ = 0.0)
        : x(x_), y(y_), z(z_) {}
        
    Vector3(std::initializer_list<double> list) {
        auto it = list.begin();
        x = (it != list.end()) ? *it : 0.0;
        y = (it + 1 != list.end()) ? *(it + 1) : 0.0;
        z = (it + 2 != list.end()) ? *(it + 2) : 0.0;
    }
    
    static Vector3 from(double x_, double y_, double z_) { return {x_, y_, z_}; }
    
    // Arithmetic Operators
    Vector3 operator+(const Vector3 &other) const { return {x + other.x, y + other.y, z + other.z}; }
    Vector3 operator-(const Vector3 &other) const { return {x - other.x, y - other.y, z - other.z}; }
    Vector3 operator*(double scalar) const { return {x * scalar, y * scalar, z * scalar}; }
    Vector3 operator/(double scalar) const { return {x / scalar, y / scalar, z / scalar}; }
    
    Vector3 &operator+=(const Vector3 &other) { x += other.x; y += other.y; z += other.z; return *this; }
    Vector3 &operator-=(const Vector3 &other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    Vector3 &operator*=(double scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vector3 &operator/=(double scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }
    
    bool operator==(const Vector3 &other) const {
        return (std::fabs(x - other.x) < EPSILON &&
                std::fabs(y - other.y) < EPSILON &&
                std::fabs(z - other.z) < EPSILON);
    }
    bool operator!=(const Vector3 &other) const { return !(*this == other); }
    
    double Magnitude() const;
    double MagnitudeSqr() const;
    Vector3 Normalized() const;
    static double DotProduct(const Vector3 &v1, const Vector3 &v2);
    static double VectorAngle(const Vector3 &v1, const Vector3 &v2);
    static Vector3 Lerp(const Vector3 &v1, const Vector3 &v2, double amount);
    static Vector3 Reflect(const Vector3 &v, const Vector3 &normal);
    static Vector3 Min(const Vector3 &v1, const Vector3 &v2);
    static Vector3 Max(const Vector3 &v1, const Vector3 &v2);
    Vector3 RotateZ(double angle);
    static Vector3 MoveTowards(const Vector3 &v, const Vector3 &target, double maxDistance);
    Vector3 Clamp(const Vector3 &min, const Vector3 &max);
    Vector3 Refract(const Vector3 &n, double r);
    static double CrossProduct(const Vector3 &v1, const Vector3 &v2);
    static double CalculateDistance(const Vector3 &first, const Vector3 &second);
};

constexpr Vector3 Vector3Up(0.0, -1.0,  0.0);        // Vector3 pointing up
constexpr Vector3 Vector3Down(0.0,  1.0,  0.0);      // Vector3 pointing down
constexpr Vector3 Vector3Left(-1.0, 0.0,  0.0);       // Vector3 pointing left
constexpr Vector3 Vector3Right(1.0,  0.0,  0.0);      // Vector3 pointing right
constexpr Vector3 Vector3Forward(0.0,  0.0,  1.0);    // Vector3 pointing forward
constexpr Vector3 Vector3Backward(0.0,  0.0, -1.0);   // Vector3 pointing backward
constexpr Vector3 Vector3One(1.0,  1.0,  1.0);        // Vector3 with all components equal to 1
constexpr Vector3 Vector3Zero(0.0,  0.0,  0.0);       // Vector3 with all components equal to 0



class Vector2 {
public:
    double x, y;
    Vector2(std::initializer_list<double> list) {
        auto it = list.begin();
        x = (it != list.end()) ? *it : 0.0;
        y = (it + 1 != list.end()) ? *(it + 1) : 0.0;
    }
    
    constexpr Vector2(double x_ = 0.0, double y_ = 0.0) : x(x_), y(y_) {}
    Vector2(const Vector3 &v3) : x(v3.x), y(v3.y) {}
    operator Vector3() const { return {x, y, 0.0}; }
    
    Vector2 operator+(const Vector2 &other) const { return {x + other.x, y + other.y}; }
    Vector2 operator-(const Vector2 &other) const { return {x - other.x, y - other.y}; }
    Vector2 operator*(double scalar) const { return {x * scalar, y * scalar}; }
    Vector2 operator/(double scalar) const { return {x / scalar, y / scalar}; }

    Vector2 &operator+=(const Vector2 &other) { x += other.x; y += other.y; return *this; }
    Vector2 &operator-=(const Vector2 &other) { x -= other.x; y -= other.y; return *this; }
    Vector2 &operator*=(double scalar) { x *= scalar; y *= scalar; return *this; }
    Vector2 &operator/=(double scalar) { x /= scalar; y /= scalar; return *this; }
    
    double Magnitude() const;
    double MagnitudeSqr() const;
    Vector2 Normalized() const;
    static double DotProduct(const Vector2 &v1, const Vector2 &v2);
    static double VectorAngle(const Vector2 &v1, const Vector2 &v2);
    static Vector2 Lerp(const Vector2 &v1, const Vector2 &v2, double amount);
    Vector2 Reflect(const Vector2 &normal);
    static Vector2 Min(const Vector2 &v1, const Vector2 &v2);
    static Vector2 Max(const Vector2 &v1, const Vector2 &v2);
    Vector2 Rotate(const Vector2 &v, double angle);
    static Vector2 MoveTowards(const Vector2 &v, const Vector2 &target, double maxDistance);
    Vector2 Clamp(const Vector2 &min, const Vector2 &max);
    Vector2 Refract(const Vector2 &n, double r);
    static double CrossProduct(const Vector2 &v1, const Vector2 &v2);
    static double CalculateDistance(const Vector2 &first, const Vector2 &second);
};

constexpr Vector2 Vector2Up(0.0, -1.0);        // Vector2 pointing up
constexpr Vector2 Vector2Down(0.0,  1.0);      // Vector2 pointing down
constexpr Vector2 Vector2Left(-1.0, 0.0);       // Vector2 pointing left
constexpr Vector2 Vector2Right(1.0,  0.0);      // Vector2 pointing right
constexpr Vector2 Vector2One(1.0,  1.0);        // Vector2 with both components equal to 1
constexpr Vector2 Vector2Zero(0.0,  0.0);       // Vector2 with both components equal to 0


class Rect {
public:
    double x, y, width, height;
    
    Rect(double x, double y, double width, double height) : x(x), y(y), width(width), height(height) {}
    Rect() : x(0), y(0), width(0), height(0) {}

    bool Contains(Vector2 point) const;
    bool Overlaps(const Rect &other) const;
    void Set(double x, double y, double width, double height);
    void Normalize();
    bool operator==(const Rect &other) const;
    bool operator!=(const Rect &other) const;
};




#endif
