# Function name : calculateDistance2, calculateDistance3

## Function prototypes

```cpp
double calculateDistance2(Vector2 first, Vector2 second);
double calculateDistance3(Vector3 first, Vector3 second);
```

## Implementation

```cpp
double Silver::calculateDistance2(Vector2 first, Vector2 second) {
    float dx = first.x - second.x;
    float dy = first.y - second.y;
    return sqrt(dx * dx + dy * dy);
}

double Silver::calculateDistance3(Vector3 first, Vector3 second) {
    float dx = first.x - second.x;
    float dy = first.y - second.y;
    float dz = first.z - second.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}
```

## Description
These functions calculate the distance between two objects. One of them (`calculateDistance3`) considers the difference of the Z axis. And the other (`calculateDistance2`) ignores it. 

## Example Usage
```cpp
const int magnetOffset = 3;
if(calculateDistance2(getLocation(magnetId) , getLocation(playerId)) <= magnetOffset)
  glideObjectToTarget(magnetId, playerId, 1000);
```

