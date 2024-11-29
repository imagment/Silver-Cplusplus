# Scene Management
## Class Structure
```cpp
class Scene {
    public: void saveWorldAs(std::string name);
    std::map < std::string, World > scenes;
    void changeSceneTo(std::string name);
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
