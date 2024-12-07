# Function name : getObjectsAt2, getObjectsAt3

## Function prototype

```cpp
std::vector<int> getObjectsAt2(Vector2 pos);
std::vector<int> getObjectsAt3(Vector3 pos);
```

## Implementation

```cpp
vector<int> Silver::getObjectsAt2(Vector2 pos) {
  vector<int> ids;

  for (auto &entry : workspace) {
    if (entry.second.position.x == pos.x && entry.second.position.y == pos.y) {

      ids.push_back(entry.first);
    }
  }

  return ids;
}
vector<int> Silver::getObjectsAt3(Vector3 pos) {
  vector<int> ids;

  for (auto &entry : workspace) {
    if (entry.second.position.x == pos.x && entry.second.position.y == pos.y &&
        entry.second.position.z == pos.z) {

      ids.push_back(entry.first);
    }
  }

  return ids;
}
```

## Description
The `getObjectsAt2` function finds meshes located at a specific position but the Z position does not matter and uses Vec2 for the position.
The `getObjectsAt3` function, on the other hand, finds meshes at a specific position and the Z axis also matters.
These functions return a vector of object IDs.

## Example Usage
```cpp
vector<int> check = Silver::getObjectsAt2(playerPosition);
for (int value : check) {
  if(value>=zombieIdMin &&  value<=zombieIdMax) gameOver();
}
```
