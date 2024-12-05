# Function name : kill

## Function prototype

```cpp
void kill (variant<int, vector<int>> objIDs);
```

## Implementation

```cpp
void Silver::kill(variant<int, vector<int>> objIDs) {
    vector<int> keysToKill;

    if (holds_alternative<int>(objIDs)) {
        int objID = get<int>(objIDs);
        if (workspace.find(objID) != workspace.end()) {
            keysToKill.push_back(objID);
        }
    } else {
        for (int objID : get<vector<int>>(objIDs)) {
            if (workspace.find(objID) != workspace.end()) {
                keysToKill.push_back(objID);
            }
        }
    }

    for (const int &key : keysToKill) {
        killedSprites[key] = workspace[key];
        workspace.erase(key);
    }
}
```

## Description
This function removes an object. It could return by using the `revive` function.

## Example Usage
```cpp
int playerId = 0;
vector <int> zombieId = collectZombies();
Silver::kill(playerId);
Silver::kill(zombieId);
```
