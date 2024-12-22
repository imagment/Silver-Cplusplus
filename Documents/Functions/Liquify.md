# Function name : Liquify

## Function prototype

```cpp
void Liquify(const std::variant<int, std::vector<int>> &IDs,
             double diffusionSpeed, int maximumDistance);
```

## Implementation

```cpp
void Silver::Fluid::Liquify(const variant < int, vector < int >> & IDs, double diffusionSpeed, int maximumDistance) {

  auto liquifyMesh = [ & ](int id) {
    if (workspace.find(id) != workspace.end()) {
      mesh & m = workspace[id];
      m.comp.isFluid = true;
      m.comp.diffusionSpeed = diffusionSpeed;
      m.comp.maximumDistance = maximumDistance;
      flow(id);
    }
  };

  if (std::holds_alternative < int > (IDs)) {
    int id = std::get < int > (IDs);
    liquifyMesh(id);
  } else if (std::holds_alternative < std::vector < int >> (IDs)) {
    for (int id: std::get < std::vector < int >> (IDs)) {
      liquifyMesh(id);
    }
  }
}

vector < int > Silver::duplicate(const variant < int, vector < int >> & IDs) {
  vector < int > duplicatedIDs;

  if (std::holds_alternative < int > (IDs)) {
    int id = std::get < int > (IDs);

    silver.sprites_count++;
    workspace[sprites_count - 1] = workspace[id];
    duplicatedIDs.push_back(sprites_count - 1);

  } else if (std::holds_alternative < vector < int >> (IDs)) {

    for (int id: std::get < vector < int >> (IDs)) {
      silver.sprites_count++;
      workspace[sprites_count - 1] = workspace[id];
      duplicatedIDs.push_back(sprites_count - 1);
    }
  }

  return duplicatedIDs;
}
```

## Description
This function makes certain object to flow in all four directions. It would copy the object and simulate flowing.

## Example Usage
```cpp
Silver::Fluid::Liquify(waterID, 5000, 5);
```
