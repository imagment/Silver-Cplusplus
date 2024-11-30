# Function name : saveWorldAs

## Function prototype

```cpp
void Silver::Scene::saveWorldAs(std::string name);
```

## Implementation

```cpp
void Silver::Scene::saveWorldAs(std::string name) {
  scenes[name] = workspace;
}
```

## Description
This function saves the current scene (workspace map) into the `scenes` map. You can change to another scene save in the `scenes` map using the `changeSceneTo` function.
## Example Usage
```cpp
silver.scene.saveWorldAs("menu");
```
