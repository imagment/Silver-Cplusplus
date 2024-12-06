# Function name : loadScene

## Function prototype

```cpp
void Silver::Scene::loadScene(const string name);
```

## Implementation

```cpp
void Silver::Scene::loadScene(const string name) {
  auto it = scenes.find(name);
  if (it != scenes.end()) {
    workspace = it->second;
  }
}
```

## Description
This function looks for a scene in the scenes map by its name. If the scene is found, it sets the current scene (workspace) to the found scene.

## Example Usage
```cpp
Silver::Scene::loadScene("menu");
```

## Alternative
This can be used instead of the loadScene function
```cpp
workspace = Silver::Scene::savedScenes["menu"];
```
