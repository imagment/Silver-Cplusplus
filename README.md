# Silver C++ Developer Documentation

[![Discord](https://img.shields.io/badge/Discord-LoTGD-blue?logo=discord)](https://discord.gg/72TbP8G69k)

## Index
- [Vectors](#Vectors)
- [Functions](#functions)
  - [Camera](#camera)
  - [Debugging](#debugging)
  - [Object Declaration](#object-declaration)
  - [Object Movement](#object-movement)
  - [General Functions](#general-functions)
    
## Vectors
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
Vector3 result = v1 + v2 + vec3.up; // Result is (4, 5, 3)
```

> Warning:
> In Silver C++, Negative Y represents up and Positive Y represents down.

## Functions

### Camera
**NOTE: All camera-related functions are in the namespace 'Camera'**

You can use this function to set the camera position:
```cpp
void setCam2(Vector2 pos, Vector2 scale);
void setCam3(Vector3 pos, Vector3 scale);
```

Example usage:
```cpp
Silver::Camera::setCam2(Vector2(10,10),Vector2(20,30)); // Does not change Z pos and scale
Silver::Camera::setCam3(Vector3(3,2,4),Vector3(6,6,8)); 
```

This sets the camera position to the first parameter `pos`, and the camera size to the second parameter `scale`.

There are a variety of functions to control the camera:

```cpp
void setCameraFlip(bool x, bool y); // true: Flipped, false: Normal
void flipCamera(bool x, bool y); // true: Toggle, false: Leave it
void pivotCamera(int angle); // Set camera's rotation
void addPivotCamera(int angle); // Adds to the camera's current rotation
void shakeCamera(float intensity); //Shakes the camera
void zoomCamera(Vector3 v); // Changes the camera scale
void moveCamera(Vector3 v); // Moves the camera in a certain offset
```

To print the camera's view you can use the following function
```cpp
void Silver::Camera::printCam();
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
Silver::Camera::startVideo(1); // Starts video at 1 frame per second
```

To stop the video:
```cpp
Silver::Camera::stopVideo();
```

### Advanced Camera Functions & Cells
Null Object Replacement (🟫): Displayed when a cell has no objects.

You can configure these using:
```cpp
void configCameraException(string n); // n = null object replacement
```
<br>
And if you want to set the world bounds, you can use the following function:
`void setWorldBounds(Vector3 world);`
This sets the world bounds to `Vector3 world`. However, this does not change the minimum X and Y coordinates to <br>
stay in the world.

**Cell Size**:
The default cell size is 2. You can adjust the cell size using:
```cpp
void cell(int c);
```

### Debugging
When you render the camera, debugging something to the console is difficult. <br>
Then, you can use the `Silver::debug` function. <br>
Example:
```
Silver::debug("Hello World!","d");

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
void draw(Vector3 pos, std::string c); 
void Line(Vector3 start, Vector3 end, std::string c);
void Rectangle(Vector3 topLeft, int width, int height, std::string c); 
void Circle(Vector3 center, int radius, std::string c); 
void CircleHollow(Vector3 center, int radius, std::string c);
void Oval(Vector3 center, int radiusX, int radiusY, string c);
void OvalHollow(Vector3 center, int radiusX, int radiusY, string c);

void sprayRectangle(int spawns, Vector3 center, Vector3 scale, string c);
void sprayOval(int spawns, Vector3 center, Vector3 scale, string c);
void spray(int spawns, Vector3 center, int range, string c);
void sprayLine(int spawns, Vector3 start, Vector3 end, string c);
```
Drawing functions are in class 'Drawing' and can accessed with 'draw' <br>
Example usage: <br>
```cpp
Silver::Drawing::draw(Vector3(2,4,3),"#");
```

<br>

### Object Declaration
To create an object, you can use the following function: <br>

``` cpp
void createEmptyPrefab(const string name);
void createPrefab(const std::string name, const std::string shape);
```
`createPrefab` creates an object named `name` and will look like `shape`. <br>
`createEmptyPrefab` creates an object named `name` and will not be shown. <br>
<br>
If you create an object, you can use one of these functions:

```cpp
int placeObject(string objectName, int number, Vector3 position); // Places an object in the world
int addObject(string objectName, Vector3 position); // Places an object in the world with a unique number
int put(string objectName, Vector3 position); // Places an object in the world with number 0.

int unoccupiedNumber(string objectName); // Returns a number that is not currently being used by any object with the same name (
```
When an object gets placed on the map, it requires a number. Also, some of the 2 objects might have the same number.
```cpp
Silver::place("player", 1, Vector2(10, 10));
Silver::place("player", 1, Vector2(10, 11)); // This is allowed
```
Objects are numbered for unique identification and manipulation within the world. So you can use the `void put(string objectName, Vector3 position);` function if the number doesn't matter much. <br>
Also, placing related functions like `placeObject`, `addObject`, and `put` return an integer. That integer is used in most of the operations. 

```cpp
void Line(Vector3 start, Vector3 end, string name, int number);
void Rectangle(Vector3 topLeft, int width, int height, string name, int number);
void Circle(Vector3 center, int radius, string name, int number);
void CircleHollow(Vector3 center, int radius, string name, int number);
void Oval(Vector3 center, int radiusX, int radiusY, string name, int number);
void OvalHollow(Vector3 center, int radiusX, int radiusY, string name, int number);

void sprayRectangle(int spawns, Vector3 center, Vector3 scale, string name, int number);
void sprayOval(int spawns, Vector3 center, Vector3 scale, string name, int number);
void spray(int spawns, Vector3 center, int range, string name, int number);
void sprayLine(int spawns, Vector3 start, Vector3 end, string name, int number);
```
Use these functions (Not in class `Drawing`) to place the same object in a certain shape.


### Object Movement

```cpp
void moveObjectXY(const variant < int, vector < int >> objectID, Vector3 pos);
void moveObjectX(const variant < int, vector < int >> objectID, int x_offset);
void moveObjectY(const variant < int, vector < int >> objectID, int y_offset);
void moveObjectPosition(const variant < int, vector < int >> objectID, Vector3 pos);

void setObjectXY(const variant < int, vector < int >> objectID, Vector3 pos);
void setObjectX(const variant < int, vector < int >> objectID, Vector3 pos);
void setObjectY(const variant < int, vector < int >> objectID, Vector3 pos);
void setObjectPosition(const variant < int, vector < int >> objectID, Vector3 pos);
void setObjectPositionToSprite(const variant < int, vector < int >> objectIDs, int spriteID);

void setObjectRandom(const variant < int, vector < int >> objectID,
const pair < int, int > & xRange,
const pair < int, int > & yRange);

void glideObjectPositionToSprite(const variant < int, vector < int >> objectIDs, int spriteID, float speed);
void glideObjectX(const variant < int, vector < int >> & ids, int x_offset, float speed, ...);
void glideObjectY(const variant < int, vector < int >> & ids, int y_offset, float speed, ...);
void glideObjectPositionToSprite(const variant < int, vector < int >> objectIDs, int spriteID, float speed);

void glideObjectRandom(const variant < int, vector < int >> & ids,
const pair < int, int > & xRange,
const pair < int, int > & yRange, float speed);

void glideObjectXY(const variant < int, vector < int >> & ids,
const Vector3 & offset, float speed, ...);
```
Check movements.md for more information

### General Functions
These are some useful functions that could be used frequently in game development. <br>
```cpp
void kill(variant<int, vector<int>> objIDs);
void revive(variant<int, vector<int>> objIDs);

void destroy(variant<int, vector<int>> objIDs);
```
The `kill` Function removes the object from the workspace and `revive` function returns it. <br>
To delete the object completely, use the `destroy` function. <br>
<br>
```cpp
mesh * getMesh(int objID);
mesh getMeshValue(int objID);
```
The `getMesh` function Returns a pointer to the mesh of a specific object.
The `getMeshValue` function Returns the mesh of a specific object.
<br>

```cpp
void hold ();
void wait (float time);
```
`hold` function waits forever, preventing the program from ending. <br>
`wait` function waits for a certain amount of time (milliseconds) <br>

```
bool isAlive(int obj);
vector < int > all();
vector < int > seek(string objectName);
vector < int > findObjects(string name, variant < vector < int > , int > number);
```
`isAlive` function checks if an object exists. <br>
`all` function returns all object IDs that exist in the current workspace <br>
`findObject` function takes two inputs:

A name (string) of an object.
A vector of numbers (or a single number).
It returns a vector of object IDs (int). These IDs correspond to objects in the workspace that match the specified name and any specified numbers. 
