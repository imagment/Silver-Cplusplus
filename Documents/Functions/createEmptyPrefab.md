# Function name : createEmptyPrefab

## Function prototype

```cpp
void Silver::createEmptyPrefab(const string name);
```

## Implementation

```cpp
void Silver::createEmptyPrefab(const string name) {
  if (prefabrications.count(name) > 0) {
    return;
  }

  prefab A;
  A.name = name;
  A.shape = "\0";
  A.isTransparent = true;
  prefabrications[name] = A;
}
```

## Description
This function is used to create a prefabrication of an object. And this object would not be shown on camera.

## Example Usage
```cpp
Silver::createObject("Death Checker");
```
