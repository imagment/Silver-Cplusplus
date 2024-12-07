# Function name : loadScene

## Function prototype

```cpp
void Silver::Scene::loadScene(const string name);
```

## Implementation

```cpp
void Silver::Scene::loadScene(const string name) {
  auto it = savedScenes.find(name);
  if (it != savedScenes.end()) {
    workspace = it->second;
  }
}
```

## Description
This function loads a saved scene in the `scene` map. You can save the current scene with the `saveCurrentWorld` function.
If you want to keep the current scene, you should save it before you load the other scene.

## Example Usage
```cpp
createMenuScene(); // Create the menu scene
Silver::Scene::saveCurrentWorld("menu");
while(canStart()) {
  Silver::wait(10); // Wait until the game can start
}
startGame();
// Show menu screen when game is over
Silver::loadScene("menu");
```

## Alternative
This can be used instead of the loadScene function
```cpp
workspace = Silver::Scene::savedScenes["menu"];
```
