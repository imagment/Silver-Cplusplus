# Function name : getMeshValuesAt2, getMeshValuesAt3

## Function prototypes

```cpp
vector < mesh > getMeshValuesAt2(Vector3 pos);
vector < mesh > getMeshValuesAt3(Vector3 pos);
```

## Implementation

```cpp
vector < mesh > Silver::getMeshValuesAt2(Vector2 pos) {
  vector < mesh > result;

  for (auto & entry: workspace) {
    if (entry.second.position.x == pos.x && entry.second.position.y == pos.y) result.push_back(entry.second);
  }

  return result;
}

vector < mesh > Silver::getMeshValuesAt3(Vector3 pos) {
  vector < mesh > result;

  for (auto & entry: workspace) {
    if (entry.second.position == pos) result.push_back(entry.second);
  }

  return result;
}
```

## Description
The `getMeshValuesAt2` function finds meshes located at a specific position but the Z position does not matter and uses Vec2 for the position.
The `getMeshValuesAt3` function, on the other hand, finds meshes at a specific position and the Z axis also matters.
These function returns a vector of meshes.

## Example Usage
```
int k = Silver::place("player", 0, Vec3(10,10,0));
mesh playerMesh = Silver::getMeshValuesAt3(Vec3(10,10,0))[0];
```
