# Function name : all

## Function prototype

```cpp
vector <int> all();
```

## Implementation

```cpp
vector<int> Silver::all() {
  vector<int> keys;
  keys.reserve(workspace.size());

  for (const auto &pair : workspace) {
    keys.push_back(pair.first);
  }

  return keys;
}
```

## Description
This function returns a vector that contains all IDs of each object.

## Example Usage
```cpp
vector<int> all = Silver::all();
for(auto it : all) {
  workspace[it].position=Vector3(0,0,0);
}
```
