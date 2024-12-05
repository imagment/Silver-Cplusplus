# Function name : createPrefab

## Function prototype

```cpp
void Silver::createPrefab(const string name, const string shape);
```

## Implementation

```cpp
void Silver::createPrefab(const string name, const string shape) {
  if (!isValidName(name)) {
    silver.debug("From \"void Silver::createObject(const string name, const string shape)\" : Invalid object name containing unprintable characters ", "e");
    return;
  }

  if (prefabrications.count(name) > 0) {
    return;
  }

  prefab A;
  A.name = name;
  A.shape = shape;
 

  prefabrications[name] = A;
}
```

## Description
This function is used to create a prefabrication of an object. This requires the name of an object and how it will look on the camera.

## Example Usage
```cpp
Silver::createObject("player","😊");
```
