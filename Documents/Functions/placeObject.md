# Function name : placeObject

## Function prototype

```cpp
int placeObject(const string objectName, int number, Vec3 position) 
```

## Implementation

```cpp
int Silver::placeObject(const string objectName, int number, Vec3 position) {
  if (number < 0) {
    number = 0;
  }
  if (prefabrications.count(objectName) == 0) {
    return -1;
  }

  auto entry = prefabrications.find(objectName);
  if (entry != prefabrications.end()) {

    mesh X=entry->second;
    X.position = position;
    X.number = number;

    silver.sprites_count++;

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
      silver.debug("Maximum Object Reached", "e");
      return -1;
    }

    workspace.insert({
      missingID,
      X
    });
    return missingID;
  }

  return -1;
}
```

## Description
This function changes a prefab to mesh (Because elements of workspace is `mesh` and `prefab` and `mesh` is compatible)  and places to the workspace. 
`objectName` is the name of prefab that you created with the `createPrefab` function. 
`number` is a number of an object that would be a numerical identifier to the object. The number does not need to be unique, 
meaning you can use the same number for multiple objects if desired. 
This numerical identifier helps in managing and referencing the objects within the workspace. 
**Make sure you don't mix up object numbers with their unique ID.**
`position` is a position of an object. Object would be placed in that position.


## Example Usage
```cpp
for(int i = 0; i < 5; i++) {
    silver.placeObject("zombies", i, zombies[i]);
}
```
