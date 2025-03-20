#include "smath.hpp"
#include <cmath>


// Reterns the length (magnitude) of the vector
double Vector3::Magnitude() const { return sqrtf(x * x + y * y + z * z); }

// Returns the squared length (magnitude squared) of the vector
double Vector3::MagnitudeSqr() const { return x * x + y * y + z * z; }

// Returns a unit vector (normalized vector) in the same direction as the vector
Vector3 Vector3::Normalized() const {
  double len = Magnitude();
  return (len > 0.0f) ? *this / len : *this; // Prevent division by zero
}

// Dot product for Vector3 (returns double)
double DotProduct(const Vector3 &v1, const Vector3 &v2) {
  return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

// Returns the angle between two vectors
double VectorAngle(const Vector3 &v1, const Vector3 &v2) {
  // Compute the dot product of the two vectors
  double dot = DotProduct(v1, v2);

  // Compute the magnitudes of the two vectors
  double magnitudeV1 = sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
  double magnitudeV2 = sqrt(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z);

  // Ensure magnitudes are not zero to avoid division by zero
  if (magnitudeV1 == 0.0 || magnitudeV2 == 0.0) {
    return 0.0; // Return 0 for degenerate vectors
  }

  // Calculate the angle in radians using acos
  double angle = acos(dot / (magnitudeV1 * magnitudeV2));

  return angle; // Return the angle in radians
}

// Linear interpolation between two Vector3 (returns Vector3)
Vector3 Lerp(const Vector3 &v1, const Vector3 &v2, double amount) {
  return {v1.x + amount * (v2.x - v1.x), v1.y + amount * (v2.y - v1.y),
          v1.z + amount * (v2.z - v1.z)};
}


// Minimum of two Vector3 components (returns Vector3)
Vector3 VectorMin(const Vector3 &v1, const Vector3 &v2) {
  return {fminf(v1.x, v2.x), fminf(v1.y, v2.y), fminf(v1.z, v2.z)};
}

// Maximum of two Vector3 components (returns Vector3)
Vector3 VectorMax(const Vector3 &v1, const Vector3 &v2) {
  return {fmaxf(v1.x, v2.x), fmaxf(v1.y, v2.y), fmaxf(v1.z, v2.z)};
}

// Rotate a Vector3 by a given angle around the Z-axis (returns Vector3)
Vector3 Vector3::RotateZ(double angle) {
  double cosres = cosf(angle);
  double sinres = sinf(angle);

  return {x * cosres - y * sinres, x * sinres + y * cosres, z}; // Z-component is unchanged
}

// Move a Vector3 towards a target with a max distance (returns Vector3)
Vector3 MoveTowards(const Vector3 &v, const Vector3 &target,
                          double maxDistance) {
  double dx = target.x - v.x;
  double dy = target.y - v.y;
  double dz = target.z - v.z;
  double distSqr = dx * dx + dy * dy + dz * dz;

  if (distSqr == 0.0f ||
      (maxDistance >= 0 && distSqr <= maxDistance * maxDistance)) {
    return target;
  }

  double dist = sqrtf(distSqr);
  return {v.x + dx / dist * maxDistance, v.y + dy / dist * maxDistance,
          v.z + dz / dist * maxDistance};
}

// Clamp a Vector3 between two other Vector3s (returns Vector3)
Vector3 Vector3::Clamp(const Vector3 &min, const Vector3 &max) {
  return {fminf(fmaxf(x, min.x), max.x), fminf(fmaxf(y, min.y), max.y),
          fminf(fmaxf(z, min.z), max.z)};
}


// Refract a Vector3 through a surface with a normal and refraction ratio
Vector3 Vector3::Refract(const Vector3 &n, double r) {
  Vector3 v = *this;
  double dot =  v.x * n.x + v.y * n.y + v.z * n.z;  
  double d = 1.0f - r * r * (1.0f - dot * dot);

  if (d >= 0.0f) {
    d = sqrtf(d);
    v.x = r * x - (r * dot + d) * n.x;
    v.y = r * y - (r * dot + d) * n.y;
    v.z = r * z - (r * dot + d) * n.z;
    return v;
  }

  return Vector3(0, 0, 0); // Return a zero vector if refraction is not possible (total internal reflection)
}

// Cross product for Vector3 (returns Vector3)
Vector3 CrossProduct(const Vector3 &v1, const Vector3 &v2) {
  return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,
          v1.x * v2.y - v1.y * v2.x);
}

// Calculate distance between two vectors
double CalculateDistance(Vector3 first, Vector3 second) {
  float dx = first.x - second.x;
  float dy = first.y - second.y;
  float dz = first.z - second.z;
  return sqrt(dx * dx + dy * dy + dz * dz);
}

// Magnitude of a Vector2 (returns double)
double Vector2::Magnitude() const {
  return sqrtf(x * x + y * y);
}

// Magnitude squared of a Vector2 (returns double)
double Vector2::MagnitudeSqr() const {
  return (x * x + y * y);
}

//  Returns a unit vector (normalized vector) in the same direction as the vector
Vector2 Vector2::Normalized() const {
  double mag = Magnitude();
  return (mag != 0.0f) ? Vector2(x, y) / mag : *this; // Avoid division by zero
}

// Dot product for Vector2 (returns double)
double DotProduct(const Vector2 &v1, const Vector2 &v2) {
  return (v1.x * v2.x + v1.y * v2.y);
}

// Angle between two Vector2 (returns double)
double VectorAngle(const Vector2 &v1, const Vector2 &v2) {
  double dot = DotProduct(v1, v2);
  double magnitude1 = v1.Magnitude();
  double magnitude2 = v2.Magnitude();

  // To prevent floating-point errors from causing issues
  double cosTheta = dot / (magnitude1 * magnitude2);
  
  // Clamp the cosine value between -1 and 1 to avoid domain errors in acos
  cosTheta = std::fmax(-1.0, std::fmin(1.0, cosTheta));

  return std::acos(cosTheta); // Returns the angle in radians
}

// Linear interpolation between two Vector2 (returns Vector2)
Vector2 Lerp(const Vector2 &v1, const Vector2 &v2, double amount) {
  return {v1.x + amount * (v2.x - v1.x), v1.y + amount * (v2.y - v1.y)};
}

// Reflection of a Vector2 off a surface with a normal (returns Vector2)
Vector2 Vector2::Reflect(const Vector2 &normal) {
  double dot = x * normal.x + y * normal.y;
  return {x - 2 * normal.x * dot, y - 2 * normal.y * dot};
}

Vector2 Min(const Vector2 &v1, const Vector2 &v2) {
    return {std::min(v1.x, v2.x), std::min(v1.y, v2.y)};
}

Vector2 Max(const Vector2 &v1, const Vector2 &v2) {
    return {std::max(v1.x, v2.x), std::max(v1.y, v2.y)};
}

Vector2 Rotate(const Vector2 &v, double angle) {
    double c = cos(angle), s = sin(angle);
    return {c * v.x - s * v.y, s * v.x + c * v.y};
}

Vector2 MoveTowards(const Vector2 &v, const Vector2 &target, double maxDistance) {
    Vector2 delta = target - v;
    double dist = delta.Magnitude();
    return dist <= maxDistance ? target : v + delta.Normalized() * maxDistance;
}

Vector2 Vector2::Clamp(const Vector2 &min, const Vector2 &max) {
    return {std::clamp(x, min.x, max.x), std::clamp(y, min.y, max.y)};
}

Vector2 Vector2::Refract(const Vector2 &n, double r) {
    double dot = x * n.x + y * n.y;
    return *this - n * (2.0 * dot * r);
}

// Calculate distance between two vectors
double CalculateDistance(Vector2 first, Vector2 second) {
  float dx = first.x - second.x;
  float dy = first.y - second.y;
  return sqrt(dx * dx + dy * dy);
}


// Contains: Check if a point (px, py) is inside the rectangle
bool Rect::Contains(Vector2 point) const {
  return (point.x >= x && point.x <= (x + width) && point.y >= y && point.y <= (y + height));
}

// Overlaps: Check if two rectangles overlap
bool Rect::Overlaps(const Rect& other) const {
  return !(other.x > (x + width) || 
           (other.x + other.width) < x || 
           other.y > (y + height) || 
           (other.y + other.height) < y);
}

// Set: Set the rectangle's x, y, width, and height
void Rect::Set(double x, double y, double width, double height) {
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
}

// Normalize: Normalize the rectangle if width or height is negative
void Rect::Normalize() {
  if (width < 0) {
    width = std::fabs(width);
    x -= width;  // Move to the left if width is negative
  }
  if (height < 0) {
    height = std::fabs(height);
    y -= height;  // Move down if height is negative
  }
}

// Operator overloading: == comparison operator
bool Rect::operator==(const Rect& other) const {
  return (x == other.x && y == other.y && 
          width == other.width && height == other.height);
}

// Operator overloading: != comparison operator
bool Rect::operator!=(const Rect& other) const {
  return !(*this == other);
}
