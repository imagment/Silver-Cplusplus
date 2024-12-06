# Movements

## Table of Contents
- [Movement 1](#movement-1)
- [Movement 2](#movement-2)
- [Gliding](#gliding)
- [Other Functions](#other-functions)

## Movement 1
Functions starting with `set` sets the current position of an object by adding the provided offsets to its current coordinates.
```cpp
void Silver::moveObjectPosition(const variant<int, vector<int>> objectID, Vector3 pos); // Updates all coordinates
void Silver::moveObjectX(const variant<int, vector<int>> objectID, int x_offset); // Updates X coordinate
void Silver::moveObjectY(const variant<int, vector<int>> objectID, int y_offset); // Updates Y coordinate
void Silver::moveObjectXY(const variant<int, vector<int>> objectID, Vector3 pos); // Only updates X and Y coordinates
```
These functions get object ID(s) and offsets. The object ID(s) can be a single integer or a vector of multiple integers. These function just adds offsets to each object's position. 

### Alternative
```cpp
workspace[playerID].position.x += 4;
workspace[playerID].position.x += 5;
// Alternative for Silver::moveObjectXY(playerID, Vector2(4,5));
```

## Movement 2
Functions starting with `move` updates the current position of an object by adding the provided offsets to its current coordinates.
```cpp
void setObjectPosition(const std::variant<int, std::vector<int>> objectID, Vector3 pos); // Only updates X and Y coordinates
void setObjectX(const std::variant<int, std::vector<int>> objectID, 
                Vector3 pos); // Updates the X coordinate
void setObjectY(const std::variant<int, std::vector<int>> objectID,
                Vector3 pos); // Updates the Y coordinate
void setObjectXY(const std::variant<int, std::vector<int>> objectID,
                 Vector2 pos); // Updates X and Y coordinates


void setObjectPositionToSprite(const std::variant<int, std::vector<int>> objectIDs, int spriteID); // Changes object's location to another object's location
void setObjectRandom(const std::variant<int, std::vector<int>> objectID,
                     const std::pair<int, int> &xRange,
                     const std::pair<int, int> &yRange); // Changes object's location to a random position
```
These functions get object ID(s) and offsets. The object ID(s) can be a single integer or a vector of multiple integers. These functions sets object's position to another position.

### Alternative
```cpp
workspace[playerID].position.x = 4;
workspace[playerID].position.x = 5;
// Alternative for Silver::setObjectXY(playerID, Vector2(4,5));
```

## Gliding
```cpp
void glideObjectX(const std::variant<int, std::vector<int>> &ids, int x_offset,
                  float speed, ...); // Only changes the X coordinate
void glideObjectY(const std::variant<int, std::vector<int>> &ids, int y_offset,
                  float speed, ...); // Only changes the Y coordinate
void glideObjectXY(const std::variant<int, std::vector<int>> &ids,
                   Vector2 offset, float speed, ...); // Updates both coordinates

void glideObjectPositionToSprite(
    const std::variant<int, std::vector<int>> objectIDs, int spriteID,
    float speed); // Glides an object to another object
void glideObjectRandom(const std::variant<int, std::vector<int>> &ids,
                       const std::pair<int, int> &xRange,
                       const std::pair<int, int> &yRange, float speed); // Glides and object to a random position
```
These functions get object ID(s) and offsets. The object ID(s) can be a single integer or a vector of multiple integers. These functions changes object's 
position and gradually move an object to its destination. <br>

### Usage
```cpp
// Original position of the player : (3,3)
Silver::glideObjectY(playerID, 14, 500); // Glides object Y position to Y=14. It would take 500 miliseconds per a step
Silver::glideObjectXY(playerID, Vec2(5,7), 500); // Glides object to (5,7). It would take 500 miliseconds per a step
Silver::glideObjectXY(playerID, Vec2(5,7), 500, true); // Because the last parameter is true, it would add (5,7) to the object's position.
```

If current position of the object is `(a,b)` `Silver::glideObjectXY(playerID, Vec2(x,y), 500, true);` and `Silver::glideObjectXY(playerID, Vec2(x,y), 500, false);`
has different effects. `Silver::glideObjectXY(playerID, Vec2(x,y), 500, true);` changes object's position to `(x,y)` but `Silver::glideObjectXY(playerID, Vec2(x,y), 500, false);`
changes object's position to `(a+x, b+y)`. 

`Silver::glideObjectXY(playerID, Vec2(x,y), 500, false);` could be also written as `Silver::glideObjectXY(playerID, Vec2(x,y), 500);`

## Other functions

