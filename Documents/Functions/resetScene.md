# Function name : resetScene

## Function prototype

```cpp
void Silver::Scene::resetScene();
```

## Implementation

```cpp
void Silver::Scene::resetScene() {
  World empty;
  workspace = empty;
}
```

## Description
This function removes all objects from the workspace.

## Example Usage
```cpp
Silver::Scene::resetScene();
printf("All objects had been removed");
```
