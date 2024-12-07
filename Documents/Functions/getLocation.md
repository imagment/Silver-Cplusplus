# Function name : getLocation

## Function prototype

```cpp
Vector3 getLocation(int id);
```

## Implementation

```cpp
Vector3 Silver::getLocation(int id) {
  auto it = workspace.find(id);
  if (it != workspace.end()) {
    return workspace[id].position;
  }
  return {-1, -1, -1};
}
```

## Description
This function gets an object ID and returns the position of that object.

## Example Usage
```cpp
if(Silver::getLocation(playerID) == Silver::getLocation(zombieID))
  Silver::kill(playerID);
```


