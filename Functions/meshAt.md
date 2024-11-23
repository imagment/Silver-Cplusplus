# Function name : meshAt2, meshAt3

## Function prototypes

```cpp
vector < mesh > MeshAt3(Vec3 pos);
vector < mesh > MeshAt2(Vec3 pos);
```

## Implementation

```cpp
vector < mesh > Silver::MeshAt2(Vec2 pos) {
  vector < mesh > result;

  for (auto & entry: workspace) {
    if (entry.second.position.x == pos.x && entry.second.position.y == pos.y) result.push_back(entry.second);
  }

  return result;
}

vector < mesh > Silver::MeshAt3(Vec3 pos) {
  vector < mesh > result;

  for (auto & entry: workspace) {
    if (entry.second.object.position == pos) result.push_back(entry.second);
  }

  return result;
}
```

## Description
This function is used to find a mesh in a certain location. The parameters are Vec2 and Vec3. <br>
They both return a vector of mesh that is in that location.

## Time complexity
Both function has a time complexity of O(N). Where N is the number of all objects.

## Example Usage
```
int k = silver.place("player", 0, Vec3(10,10,0));
mesh playerMesh = silver.MeshAt(Vec3(10,10,0));
```
