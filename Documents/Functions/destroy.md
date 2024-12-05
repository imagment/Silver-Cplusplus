# Function name : destroy

## Function prototype

```cpp
void destroy(variant<int, vector<int>> objIDs);
```

## Implementation

```cpp
void Silver::destroy(variant<int, vector<int>> objIDs) {
    vector<int> keysToRemove;

    if (holds_alternative<int>(objIDs)) {
        int objID = get<int>(objIDs);
        if (workspace.find(objID) != workspace.end()) {
            keysToRemove.push_back(objID);
        }
    } else {
        for (int objID : get<vector<int>>(objIDs)) {
            if (workspace.find(objID) != workspace.end()) {
                keysToRemove.push_back(objID);
            }
        }
    }

    for (const int &key : keysToRemove) {
        workspace.erase(key);
    }
}
```

## Description
This function removes certain objects from the workspace. Removed objects no longer exsist. 

## Example Usage
```cpp
Silver::destroy(playerId);
Silver::debug("Game Over" , "d");
```

## Alternative
```cpp
workspace.erase(id);
```
