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
This function sets the world's bounds. If the out_of_bounds option is enabled in the camera, areas outside the defined world bounds will be displayed differently.
Additionally, if you plan to use only a specific part of the world, it is recommended that you define it clearly for better usability.

## Example Usage
```cpp
silver.setWorldBounds(Vec2(100,100));
```
