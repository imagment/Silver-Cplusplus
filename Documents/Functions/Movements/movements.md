# Movements

## Table of Contents
- [Movement 1](#movement-1)
- [Movement 2](#movement-2)
- [Gliding](#gliding)
- [Other Functions](#other-functions)
- [Details](#details)

## Movement 1
Functions starting with `set` set the current position of an object by adding the provided offsets to its current coordinates.
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


void setObjectPositionToTarget(const std::variant<int, std::vector<int>> objectIDs, int spriteID); // Changes object's location to another object's location
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

void glideObjectPositionToTarget(
    const std::variant<int, std::vector<int>> objectIDs, int spriteID,
    float speed); // Glides an object to another object
void glideObjectRandom(const std::variant<int, std::vector<int>> &ids,
                       const std::pair<int, int> &xRange,
                       const std::pair<int, int> &yRange, float speed); // Glides an object to a random position
```
These functions get object ID(s) and offsets. The object ID(s) can be a single integer or a vector of multiple integers. These functions change an object's
position and gradually move it to its destination. <br>

### Usage
```cpp
// Original position of the player : (3,3)
Silver::glideObjectY(playerID, 14, 5000); // Glides object Y position to Y=14. It would take 5000 miliseconds.
Silver::glideObjectXY(playerID, Vec2(5,7), 5000); // Glides object to (5,7). It would take 5000 miliseconds.
Silver::glideObjectXY(playerID, Vec2(5,7), 5000, true); // Because the last parameter is true, it would add (5,7) to the object's position.
```

If current position of the object is `(a,b)` `Silver::glideObjectXY(playerID, Vec2(x,y), 5000, true);` and `Silver::glideObjectXY(playerID, Vec2(x,y), 5000, false);`
has different effects. `Silver::glideObjectXY(playerID, Vec2(x,y), 5000, true);` changes object's position to `(x,y)` but `Silver::glideObjectXY(playerID, Vec2(x,y), 5000, false);`
changes object's position to `(a+x, b+y)`. 

`Silver::glideObjectXY(playerID, Vec2(x,y), 5000, false);` could be also written as `Silver::glideObjectXY(playerID, Vec2(x,y), 5000);`

When using the gliding function to multiple sprites, they will arrive at their destination in unison

## Other functions
```cpp
void mesh::translate2(Vector2 offset);
void mesh::translate3(Vector3 offset);
```
These functions are member functions of the `mesh` class and their functions. Their functions are similar to functions starting with `move`.
### Usage
```cpp
workspace[playerId].translate3(Vector3(x,y,z));
```
## Alternative
You can modify someMesh.object.position instead of using these functions.
```cpp
workspace[objectId] = Vector3.zero();
```

## Details
| **Function**                                                                                                   | **Description**                                                                                                                                              | **Parameters**                                                                                                                                                                  | **Usage Example**                                                                                                                                                                         |
|---------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `void Silver::moveObjectPosition(const variant<int, vector<int>> objectID, Vector3 pos);`                    | Updates all coordinates of the object(s) by adding offsets.                                                                                                 | `objectID`: Single object ID or vector of IDs.<br>`pos`: Offset for X, Y, Z.                                                                                                   | `moveObjectPosition(playerID, Vector3(10, 20, 0));` // Adds (10, 20) to object's current position.                                                                                         |
| `void Silver::moveObjectX(const variant<int, vector<int>> objectID, int x_offset);`                          | Updates the X coordinate of the object(s) by adding an offset.                                                                                              | `objectID`: Single object ID or vector of IDs.<br>`x_offset`: Offset for X.                                                                                                   | `moveObjectX(playerID, 15);` // Adds 15 to the X coordinate.                                                                                                                               |
| `void Silver::moveObjectY(const variant<int, vector<int>> objectID, int y_offset);`                          | Updates the Y coordinate of the object(s) by adding an offset.                                                                                              | `objectID`: Single object ID or vector of IDs.<br>`y_offset`: Offset for Y.                                                                                                   | `moveObjectY(playerID, -10);` // Subtracts 10 from the Y coordinate.                                                                                                                       |
| `void Silver::moveObjectXY(const variant<int, vector<int>> objectID, Vector3 pos);`                          | Updates only the X and Y coordinates of the object(s) by adding offsets.                                                                                    | `objectID`: Single object ID or vector of IDs.<br>`pos`: Offset for X, Y.                                                                                                     | `moveObjectXY(playerID, Vector3(5, 7, 0));` // Adds (5, 7) to object's position.                                                                                                          |
| `void setObjectPosition(const variant<int, vector<int>> objectID, Vector3 pos);`                             | Sets the position of the object(s) to the specified coordinates.                                                                                            | `objectID`: Single object ID or vector of IDs.<br>`pos`: New position (X, Y, Z).                                                                                            | `setObjectPosition(playerID, Vector3(100, 200, 0));` // Sets position to (100, 200).                                                                                                       |
| `void setObjectX(const variant<int, vector<int>> objectID, Vector3 pos);`                                     | Sets the X coordinate of the object(s) to the specified value.                                                                                               | `objectID`: Single object ID or vector of IDs.<br>`pos`: New X coordinate.                                                                                                   | `setObjectX(playerID, Vector3(50, 0, 0));` // Sets X coordinate to 50.                                                                                                                      |
| `void setObjectY(const variant<int, vector<int>> objectID, Vector3 pos);`                                     | Sets the Y coordinate of the object(s) to the specified value.                                                                                               | `objectID`: Single object ID or vector of IDs.<br>`pos`: New Y coordinate.                                                                                                   | `setObjectY(playerID, Vector3(0, 75, 0));` // Sets Y coordinate to 75.                                                                                                                      |
| `void setObjectXY(const variant<int, vector<int>> objectID, Vector2 pos);`                                    | Sets the X and Y coordinates of the object(s) to the specified values.                                                                                       | `objectID`: Single object ID or vector of IDs.<br>`pos`: New X, Y coordinates.                                                                                              | `setObjectXY(playerID, Vector2(30, 60));` // Sets position to (30, 60).                                                                                                                     |
| `void setObjectPositionToTarget(const variant<int, vector<int>> objectIDs, int spriteID);`                   | Changes the position of the object(s) to match another sprite's position.                                                                                   | `objectIDs`: Single object ID or vector of IDs.<br>`spriteID`: ID of the reference sprite.                                                                                  | `setObjectPositionToTarget(playerID, spriteID);` // Sets position to match `spriteID`.                                                                                                     |
| `void setObjectRandom(const variant<int, vector<int>> objectID, const pair<int, int> &xRange, const pair<int, int> &yRange);` | Sets the position of the object(s) to a random position within the specified X and Y ranges.                                                                | `objectID`: Single object ID or vector of IDs.<br>`xRange`: Range for X.<br>`yRange`: Range for Y.                                                                           | `setObjectRandom(playerID, {0, 100}, {0, 50});` // Sets position randomly within (0, 100) for X and (0, 50) for Y.                                                                           |
| `void glideObjectX(const variant<int, vector<int>> &ids, int x_offset, float speed, ...);`                  | Gradually changes the X coordinate of the object(s) by the specified offset at the given speed.                                                              | `ids`: Single object ID or vector of IDs.<br>`x_offset`: Offset for X.<br>`speed`: Glide speed in milliseconds.<br>`...`: Optional additive flag (true/false).                | `glideObjectX(playerID, 20, 5000);` // Moves the X coordinate to X+20 over 5 seconds.                                                                                                      |
| `void glideObjectY(const variant<int, vector<int>> &ids, int y_offset, float speed, ...);`                  | Gradually changes the Y coordinate of the object(s) by the specified offset at the given speed.                                                              | `ids`: Single object ID or vector of IDs.<br>`y_offset`: Offset for Y.<br>`speed`: Glide speed in milliseconds.<br>`...`: Optional additive flag (true/false).                | `glideObjectY(playerID, 15, 3000);` // Moves the Y coordinate to Y+15 over 3 seconds.                                                                                                      |
| `void glideObjectXY(const variant<int, vector<int>> &ids, Vector2 offset, float speed, ...);`               | Gradually changes the X and Y coordinates of the object(s) by the specified offsets at the given speed.                                                     | `ids`: Single object ID or vector of IDs.<br>`offset`: Offset for X, Y.<br>`speed`: Glide speed in milliseconds.<br>`...`: Optional additive flag (true/false).               | `glideObjectXY(playerID, Vector2(10, 10), 4000, true);` // Adds (10, 10) to position and glides over 4 seconds.                                                                             |
| `void glideObjectPositionToTarget(const variant<int, vector<int>> objectIDs, int spriteID, float speed);`   | Gradually moves the object(s) to the position of another sprite at the given speed.                                                                          | `objectIDs`: Single object ID or vector of IDs.<br>`spriteID`: ID of the reference sprite.<br>`speed`: Glide speed in milliseconds.
