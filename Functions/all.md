# Function name : all

## Function prototype

```cpp
vector < int > all();
```

## Implementation

```cpp
vector < int > Silver::all() {
  std::vector < int > keys;
  keys.reserve(workspace.size());

  for (const auto & pair: workspace) {
    keys.push_back(pair.first);
  }

  return keys;
}
```

## Description
This function returns a vector that contains all IDs of each object.

## Example Usage
```cpp
vector<int> all = silver.all();
for(auto it : all) {
  workspace[it].position=Vec3(0,0,0);
}
```
