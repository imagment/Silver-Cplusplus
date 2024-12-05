# Function name : placeObject

## Function prototype

```cpp
int Silver::placeObject(const string objectName, int number, Vector3 position);
```

## Implementation

```cpp
int Silver::placeObject(const string objectName, int number, Vector3 position) {
  if (number < 0) {
    number = 0;
  }
  if (prefabrications.count(objectName) == 0) {
    return -1;
  }

  auto entry = prefabrications.find(objectName);
  if (entry != prefabrications.end()) {

    mesh X = entry->second;
    X.position = position;
    X.number = number;

    Silver::sprites_count++;

    int missingID = -1;
    if (Silver::sprites_count != numeric_limits<int>::max()) {
      missingID = Silver::sprites_count++;
    } else {
      for (int id = 0; id < numeric_limits<int>::max(); ++id) {
        if (workspace.find(id) == workspace.end()) {
          missingID = id;
          break;
        }
      }
    }

    if (missingID == -1) {
      Silver::debug("Maximum Object Reached", "e");
      return -1;
    }

    workspace.insert({missingID, X});
    return missingID;
  }

  return -1;
}
```

## Description
This function changes a prefab to mesh (Because workspace elements are `mesh` and `prefab` and `mesh` are compatible)  and places to the workspace. 
`objectName` is the name of the prefab that you created with the `createPrefab` function. 
`number` is a number of an object that would be a numerical identifier of the object. The number does not need to be unique, 
meaning you can use the same number for multiple objects if desired. 
This numerical identifier helps manage and reference the objects within the workspace. 
**Make sure you don't mix up object numbers with their unique ID.**
`position` is the position of an object. The object would be placed in that position.


## Example Usage
```cpp
for(int i = 0; i < 5; i++) {
    Silver::placeObject("zombies", i, zombies[i]);
}
```
