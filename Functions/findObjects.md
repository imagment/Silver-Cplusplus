# Function name : findObjects

## Function prototype

```cpp
vector < int > findObjects(std::string name, variant < vector < int > , int > number);
```

## Implementation

```cpp
vector < int > Silver::findObjects(string name, variant < vector < int > , int > number) {
  vector < int > numbers;

  for (auto & entry: workspace) {

    if (entry.second.name == name) {

      if (holds_alternative < vector < int >> (number)) {
        vector < int > numList = get < vector < int >> (number);

        if (find(numList.begin(), numList.end(), entry.second.number) != numList.end()) {
          numbers.push_back(entry.first);
        }
      } else if (holds_alternative < int > (number)) {
        int num = get < int > (number);

        if (entry.second.number == num || num == all_numbers) {
          numbers.push_back(entry.first);
        }
      }
    }
  }

  return numbers;
}
```

## Description
This function is used to find IDs of objects with a certain name and a certain number. <br>
If the number is all_numbers, it returns all objects with that name regardless of the number. <br>

## Example Usage
```cpp
silver.place("player",5,Vec3(10,10,0));
int playerID = silver.findObjects("player",5)[0];
```
