> Warning: <br>
> This documentation is outdated. <br>
> The documentation is getting modified. <br>
> Thanks for your patience. <br>

# game.hpp Documentation

## Index

- [Functions](#functions)
  - [Vectors](#vectors)
  - [Camera](#camera)
  - [Debugging](#debugging)
  - [Object Declaration](#object-declaration)
  - [Object Movement](#object-movement)

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
Null Object Replacement (🧱)**: Displayed when a cell has no objects.

You can configure these using:
```cpp
void configCameraException(string n); // n = null object replacement
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

### Debugging
When you are rendering the camera, debugging something to the console is not easy. <br>
Then, you can use `silver.debug` function. <br>
Example:
```
silver.debug("Hello World!","d");

```
"d" is for debug, and there are other types of debugs. <br>
"w" : Warning <br>
"p" or "d" : Debug <br>
"s" : Subtraction <br>
"q" : Question <br>
"e" : Error <br>
You can put a name of a `.ico` file instead of those abbreviations <br>
You can use capital letters when you write those abbreviations <br>
<br>
How this using this function will notify you <br>
![屏幕截图 2024-11-18 162942](https://github.com/user-attachments/assets/579be763-35dd-4ea4-9049-4f2a15996012)

### Object Declaration

To draw objects on the map, use the following functions:

```cpp
void Drawing::draw(Vec3 pos, std::string c); // Draws a string at position pos
void Drawing::Line(Vec3 start, Vec3 end, std::string c); // Draws a line
void Drawing::Rectangle(Vec3 topLeft, int width, int height, std::string c); // Draws a rectangle
void Drawing::Circle(Vec3 center, int radius, std::string c); // Draws a filled circle
void Drawing::CircleHollow(Vec3 center, int radius, std::string c); // Draws a hollow circle
void Drawing::Oval(Vec3 center, int radiusX, int radiusY, string c);
void Drawing::OvalHollow(Vec3 center, int radiusX, int radiusY, string c);

void Drawing::sprayRectangle(int spawns, Vec3 center, Vec3 scale, string c);
void Drawing::sprayOval(int spawns, Vec3 center, Vec3 scale, string c);
void Drawing::spray(int spawns, Vec3 center, int range, string c);
void Drawing::sprayLine(int spawns, Vec3 start, Vec3 end, string c);
```
Drawing functions are in class 'Drawing' and can accessed with 'draw' <br>
Example usage: <br>
```cpp
draw.draw(Vec3(2,4,3),"#");
```

<br>
To create an object, you can use the following function: <br>
``` cpp
void createObject(const std::string name, const std::string& shape);
```
This creates an object named `std::string name` and will look like `std::string& shape`. <br>
<br>
If you create an object, you can use one of these functions:

```cpp
int place(string objectName, int number, Vec3 position); // Places an object in the world
int put(string objectName, Vec3 position); // Places an object in the world with a unique number
```
When an object gets placed on the map, it requires a number. Also, some of the 2 objects might have the same number.
```cpp
silver.place("player", 1, Vec2(10, 10));
silver.place("player", 1, Vec2(10, 11)); // This is allowed
```
Objects are numbered for unique identification and manipulation within the world. So you can use the `void put(string objectName, Vec3 position);` function if the number doesn't matter much. <br>
Also, placing functions like `place` and `put` returns an integer. That integer is used in doing most of the operations.

### General Functions

### Object Movement

```cpp
void moveObjectXY(const variant < int, vector < int >> objectID, Vec3 pos);
  void moveObjectX(const variant < int, vector < int >> objectID, int x_offset);
  void moveObjectY(const variant < int, vector < int >> objectID, int y_offset);
void moveObjectPosition(const variant < int, vector < int >> objectID, Vec3 pos);


void setObjectRandom(const variant < int, vector < int >> objectID,
const pair < int, int > & xRange,
const pair < int, int > & yRange);

void setObjectXY(const variant < int, vector < int >> objectID, Vec3 pos);
void setObjectX(const variant < int, vector < int >> objectID, Vec3 pos);
void setObjectY(const variant < int, vector < int >> objectID, Vec3 pos);
void setObjectPosition(const variant < int, vector < int >> objectID, Vec3 pos);
void setObjectPositionToSprite(const variant < int, vector < int >> objectIDs, int spriteID);

void glideObjectPositionToSprite(const variant < int, vector < int >> objectIDs, int spriteID, float speed);
void glideObjectX(const variant < int, vector < int >> & ids, int x_offset, float speed, ...);
void glideObjectY(const variant < int, vector < int >> & ids, int y_offset, float speed, ...);
void glideObjectPositionToSprite(const variant < int, vector < int >> objectIDs, int spriteID, float speed);

void glideObjectRandom(const variant < int, vector < int >> & ids,
const pair < int, int > & xRange,
const pair < int, int > & yRange, float speed);

void glideObjectXY(const variant < int, vector < int >> & ids,
const Vec3 & offset, float speed, ...);
```
Check movements.md for more information
