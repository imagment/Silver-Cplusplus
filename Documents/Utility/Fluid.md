# Fluid
## Structure
```cpp
class Fluid {
void Liquify(const std::variant<int, std::vector<int>> &IDs,
             double diffusionSpeed, int maximumDistance);
void Solidify(const std::variant<int, std::vector<int>> &IDs);
};
```

## Explanation
Fluid functions can set a certain object into 'fluid' and that object will flow in four directions.

## Function explanation
`void Liquify(const std::variant<int, std::vector<int>> &IDs, double diffusionSpeed, int maximumDistance);` sets a certain object to a fluid.
`void Solidify(const std::variant<int, std::vector<int>> &IDs);` sets a certain object to fluid-blocking object.
