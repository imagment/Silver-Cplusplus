# Vectors in Silver C++

### Vectors
Vector classes provide basic vector arithmetic operations and predefined directional vectors for 3D space.

#### Class: Vector2
**Description**: The Vec2 class represents a vector in 2D space with x and y components. It supports basic vector arithmetic operations such as addition, subtraction, and scalar multiplication.

```cpp
Vector2(int x, int y) // Creates a 2D vector with specified x and y components.
```

Conversion to Vec3: Vec2 can automatically convert to Vec3. When converted, the z component of the resulting Vec3 is set to 0.

#### Class: Vec3
**Description**: The Vec3 class represents a vector in 3D space with x, y, and z components. It supports vector addition, subtraction, and scalar multiplication, and includes predefined directional vectors for common directions like up, down, left, right, forward, and backward.

**Constructors**:
```cpp
Vector3(int x, int y, int z) // Creates a 3D vector with specified x, y, and z components.
```

**Predefined Directions**:
```cpp
Vector3::up() // Returns a vector (0, -1, 0) representing the upward direction.
Vector3::down() // Returns a vector (0, 1, 0) representing the downward direction.
Vector3::left() // Returns a vector (-1, 0, 0) representing the left direction.
Vector3::right() // Returns a vector (1, 0, 0) representing the right direction.
Vector3::forward() // Returns a vector (0, 0, 1) representing the forward direction.
Vector3::backward() // Returns a vector (0, 0, -1) representing the backward direction.
Vector3::zero() // Returns a vector (0, 0, 0) representing the origin or no movement.

Vector2::up() // Returns a vector (0, -1) representing the upward direction.
Vector2::down() // Returns a vector (0, 1) representing the downward direction.
Vector2::left() // Returns a vector (-1, 0) representing the left direction.
Vector2::right() // Returns a vector (1, 0) representing the right direction.
```

**Example**:
```cpp
Vector2 v1(3, 4);
Vector3 v2(1, 2, 3);
Vector3 result = v1 + v2 + vec3.up; // result is (4, 5, 3)
```

> Warning:
> In Silver C++, Negative Y represents up and Positive Y represents down.
