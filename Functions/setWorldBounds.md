# Function name : createPrefab

## Function prototype

```cpp
void Silver::setWorldBounds(Vec2 world);
```

## Implementation

```cpp
void Silver::setWorldBounds(Vec2 world) {
  WorldX = world.x;
  WorldY = world.y;
}
```

## Description
This function is used to set the bounds of the world. If the out_of_bounds option is enabled in the camera, areas outside the defined world bounds will be displayed differently.
Additionally, if you plan to use only a specific part of the world, it is recommended to define it clearly for better usability.

## Example Usage
```cpp
silver.createObject("player","😊");
```
