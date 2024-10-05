# game.hpp Documentation

## Index

- [Functions](#functions)
  - [Vectors](#vectors)
  - [Camera](#camera)
  - [Object Declaration](#object-declaration)
  - [Object Movement](#object-movement)
- [Source Code & Modding Guidelines](#source-code--modding-guidelines)
- [List of functions](#list-of-functions)

## Functions

### Vectors

This section covers the Vec2 and Vec3 classes, respectively, representing 2D and 3D vectors.
These classes provide basic vector arithmetic operations and predefined directional vectors for 3D space.

#### Class: Vec2
**Description**: The Vec2 class represents a vector in 2D space with x and y components. It supports basic vector arithmetic operations such as addition, subtraction, and scalar multiplication.

```cpp
Vec2(int x, int y) // Creates a 2D vector with specified x and y components.
```

Conversion to Vec3: Vec2 can automatically convert to Vec3. When converted, the z component of the resulting Vec3 is set to 0.

#### Class: Vec3
**Description**: The Vec3 class represents a vector in 3D space with x, y, and z components. It supports vector addition, subtraction, scalar multiplication, and includes predefined directional vectors for common directions like up, down, left, right, forward, and backward.

**Constructors**:
```cpp
Vec3(int x, int y, int z) // Creates a 3D vector with specified x, y, and z components.
```

**Predefined Directions**:
```cpp
vector.up() // Returns a vector (0, 1, 0) representing the upward direction.
vector.down() // Returns a vector (0, -1, 0) representing the downward direction.
vector.left() // Returns a vector (-1, 0, 0) representing the left direction.
vector.right() // Returns a vector (1, 0, 0) representing the right direction.
vector.forward() // Returns a vector (0, 0, 1) representing the forward direction.
vector.backward() // Returns a vector (0, 0, -1) representing the backward direction.
vector.zero() // Returns a vector (0, 0, 0) representing the origin or no movement.
```

**Example**:
```cpp
Vec2 v1(3, 4);
Vec3 v2(1, 2, 3);
Vec3 result = v1 + v2 + vec3.up; // result is (4, 7, 3)
```

**NOTE**: While the Vec2 class is available for convenience when working with 2D vectors, all vector operations and parameters in the system ultimately use Vec3. Whenever a Vec2 is used, it is automatically converted into a Vec3 by setting the z component to 0.

### Camera
**NOTE: All camera-related functions are in the class 'Camera'**

You can use this function to set the camera position:
```cpp
void setCam(Vec3 pos, Vec3 scale, int depth);
```

Example usage:
```cpp
silver.camera.setCam3(Vec3(int,int,int),Vec3(int,int,int)); 
silver.camera.setCam2(Vec2(int,int),Vec2(int,int)); // Does not change Z pos and Z scale
```

This sets the camera position to the first parameter `pos`, the camera size to the second parameter `scale`, and the camera depth to the third parameter `depth`.

There are a variety of functions to control the camera:

```cpp
void flipCamera(int X, int Y); // 1 not toggle, -1 toggle
void SetCameraFlip(int X, int Y); // 1 normal, -1 mirror
void pivotCamera(int angle); // rotates the camera
void addPivotCamera(int angle); // adds to the camera's current rotation
void shakeCamera(float intensity); // shakes the camera
void zoomCamera(Vec3 V); // makes the camera larger
void addCameraDepth(int X); // increases the maximum depth that the camera can see
void setCameraDepth(int X); // sets the maximum depth that the camera can see
void moveCamera(Vec3 V); // moves the camera
```

To print the camera's view:
```cpp
void photo(); // prints camera's view
std::vector<std::vector<std::string>> gPhoto(); // returns the camera view as a vector
```

**Example usage**:
```cpp
while (1) {
  silver.camera.printCam();
  silver.wait(1000);
}
```

If you don't want to use a while loop or need better efficiency, you can use:
```cpp
silver.camera.startVideo(1); // Starts video at 1 frame per second
```

To stop the video:
```cpp
silver.camera.stopVideo();
```

### Advanced Camera Functions & Cells
There are two types of camera exceptions when cells are empty:
1. **Null Object Replacement (🧱)**: Displayed when a cell has no objects.
2. **Out-of-bounds Replacement (🟦)**: Displayed when a cell is out of the world range and when a cell has no objects.

You can configure these using:
```cpp
void configCameraException(string o, string n); // o = out-of-bounds, n = null object replacement
```
<br>
And if you want to set the world bounds, you can use the following function:
`void setWorldBounds(Vec3 world);`
This sets the world bounds to `Vec3 world`. However, this does not change the minimum X and Y coordinates to <br>
stay in the world.

**Cell Size**:
The default cell size is 2. You can adjust the cell size using:
```cpp
void cell(int c);
```

### Object Declaration

To draw objects on the map, use the following functions:

```cpp
void draw(Vec3 pos, std::string c); // Draws a string at position pos
void drawLine(Vec3 start, Vec3 end, std::string c); // Draws a line
void drawRectangle(Vec3 topLeft, int width, int height, std::string c); // Draws a rectangle
void drawCircle(Vec3 center, int radius, std::string c); // Draws a filled circle
void drawCircleHollow(Vec3 center, int radius, std::string c); // Draws a hollow circle
void drawRectangleHollow(Vec3 topLeft, int width, int height, std::string c); // Draws a hollow rectangle
```
However, drawings on the map cannot function. <br>
<br>
To create an object, you can use the following function: 
```cpp
void createObject(const std::string name, const std::string& shape);
```
This creates an object named `std::string name` and will look like `std::string& shape`. <br>
<br>
If you create an object, you can use one of these functions:
```cpp
void place(string objectName, int number, Vec3 position); // Places an object in the world
void put(string objectName, Vec3 position); // Places an object in the world with Number 0
void placeSpray(const string objectName, int spawns, Vec3 center, int range);  // Sprays the object
void placeExactSpray(const string objectName, int count, Vec3 startPosition); // Sprays the object. But object numbers does not change
```
When an object gets placed on the map, it requires a number. Also, some of the 2 objects might have the same number.
```cpp
silver.place("player", 1, Vec2(10, 10));
silver.place("player", 1, Vec2(10, 11)); // This is allowed
```
Objects are numbered for unique identification and manipulation within the world. So you can use `void put(string objectName, Vec3 position);` function if that doesn't matter much.

### Object Movement

```cpp
void setObjectPositionX(const string& name, const variant<int, vector<int>>& number, Vec3 pos);
void setObjectPositionY(const string& name, const variant<int, vector<int>>& number, Vec3 pos);
void setObjectPositionXY(const string& name, const variant<int, vector<int>>& number, Vec3 pos);
void setObjectPosition(const std::string name, int number, Vec3 pos);

void setObjectPositionRandom(const std::string& name, const std::variant<int, std::vector<int>>& number);
void setObjectPositionToSprite(const std::string& name, int number, const std::string& targetName, int targetNumber);

void moveObjectPositionX(string name, int number, int x_offset);
void moveObjectPositionY(string name, int number, int y_offset);
void moveObjectPositionXY(const std::string name, int number, Vec3 pos);
void moveObjectPosition(const std::string name, int number, Vec3 pos);

void glideObjectPositionRandom(const string& name, variant<int, vector <int>>& number, Vec3 position, float speed);
void glideObjectPositionX(string name, const std::variant<int, std::vector<int>>& number, int x_offset, float speed);
void glideObjectPositionY(string name, const variant<int, vector<int>>& number, int y_offset, float speed);
void glideObjectPositionXY(string name, const variant <vector <int>, int>& number, Vec3 target_pos, float speed);
void glideObjectPositionToSprite(const string& name, const variant<int, vector<int>>& number,
                                         const string& target, int targetNumber, float speed);
```
To move an object, <br>
- Put the name in `const string& name`.
- Put the number in `variant<int, vector <int>>& number`.
 - Write `all` in `variant<int, vector <int>>& number` to move all objects with any number.
 - Assign a vector of numbers in `variant<int, vector<int>>& number` to target specific objects with those numbers.
- Write the vector in `Vec3 position`.
- If this is a gliding function, write speed on `float speed`.
<br>
This would move the object to its destination.
