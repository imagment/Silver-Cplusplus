# Function name : createPrefab

## Function prototype

```cpp
void createPrefab(const std::string name, const std::string shape);
```

## Implementation

```cpp
void Silver::createPrefab(const string name, const string shape) {

  if (!isValidName(name)) {
    silver.debug("From \"void Silver::createObject(const string& name, const string& shape)\"\nInvalid object name containing unprintable characters ", "e");
    return;
  }

  if (prefabrications.count(name) > 0) {
    return;
  }

  prefab A;
  A.name = name;
  A.shape = shape;
  A.transparency = 0;

  prefabrications[name] = A;
}
```

## Description
This function is used to create a prefabrication of an object. This requires the name of an object and how it will look on the camera.

## Example Usage
```cpp
silver.createObject("player","😊");
```
