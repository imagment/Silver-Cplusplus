# Scene Management
## Structure
```cpp
namespace Scene {
  extern std::map<std::string, World> scenes;
  void saveWorldAs(const std::string name);
  void changeSceneTo(const std::string name);
  void resetScene();
};
```
## Functions
```cpp
void Silver::Scene::saveWorldAs(std::string name) {
  scenes[name] = workspace;
}

void Silver::Scene::changeSceneTo(std::string name) {
  auto it = scenes.find(name);
  if (it != scenes.end()) {
    workspace = it -> second;
  }
}

void Silver::Scene::resetScene() {
  World empty;
  workspace = empty;
}
```

## Function explanation
`saveWorldAs`: Saves the workspace into the `scenes` map. <br>
`changeSceneTo`: Get the workspace in the `scenes` map and set that map to the current workspace. <br>
`resetScene`: Resets the current scene.

## Other members
```cpp
std::map <std::string, World> scenes;
```
This is a map that contains saved scenes. It's key is the name of the scene and it returns a workspace.
