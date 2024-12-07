# Function name : saveCurrentWorld

## Function prototype

```cpp
void Silver::Scene::saveCurrentWorld(std::string name);
```

## Implementation

```cpp
void Silver::Scene::saveCurrentWorld(std::string name) {  scenes[name] = workspace; }
```

## Description
This function saves the current scene (workspace map) into the `scenes` map. You can change to another scene save in the `scenes` map using the `changeSceneTo` function.
## Example Usage
```cpp
Silver::Scene::saveCurrentScene("menu");
```
## Alternative
Instead of using the current function, you can save the current world state in a map. The map can have an integer as the key and either a mesh or World type as the value. For example:
```cpp
World menu = workspace;
```
In this example, workspace (which could be a reference to the current world) is saved into the menu data structure.

