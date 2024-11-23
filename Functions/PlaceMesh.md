# Function name : placeMesh

## Function prototype

```cpp
void PlaceMesh(mesh m, Vec3 pos);
```

## Implementation

```cpp
int Silver::PlaceMesh(mesh m, Vec3 pos) {
  int missingID = -1;

  if(silver.sprites_count != std::numeric_limits<int>::max()) {
    missingID = silver.sprites_count++;
  } else {
    for (int id = 0; id < std::numeric_limits<int>::max(); ++id) {
      if (workspace.find(id) == workspace.end()) {
        missingID = id;
        break;
      }
    }
  }

  if (missingID == -1) {
    silver.debug("Maximum number of Object Reached", "e");
    return -1;
  }

  m.position = pos;

  // Insert the mesh into the workspace using the found missingID
  workspace[missingID] = m;

  return missingID;
}
```

## Description
This function is used to place a mesh value into the world.

## Example Usage
```cpp
mesh p = playerMesh;
silver.PlaceMesh (p, Vec3(2,4,8));
```

## Are you looking for...?
If you want to put a mesh value into the workspace, you can put mesh directly into the workspace map like this:
```cpp
mesh p = playerMesh;
workspace[playerID] = p;
```

## Warning
It can place a maximum of 2^31 objects into the workspace, but we recommend putting less than 10^6 objects into the workspace. <br>
