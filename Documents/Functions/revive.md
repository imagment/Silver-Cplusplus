# Function name : revive

## Function prototype

```cpp
void revive(variant<int, vector<int>> objIDs);
```

## Implementation

```cpp
void Silver::revive(variant<int, vector<int>> objIDs) {
    vector<int> keysToRevive;

    if (holds_alternative<int>(objIDs)) {
        int objID = get<int>(objIDs);
        if (killedSprites.find(objID) != killedSprites.end()) {
            keysToRevive.push_back(objID);
        }
    } else {
        for (int objID : get<vector<int>>(objIDs)) {
            if (killedSprites.find(objID) != killedSprites.end()) {
                keysToRevive.push_back(objID);
            }
        }
    }

    for (const int &key : keysToRevive) {
        workspace[key] = killedSprites[key];
        killedSprites.erase(key);
    }
}
```

## Description
This function revives an object previously removed using the kill function and brings it back to the workspace.

## Example Usage
```cpp
silver.kill(wallId);
silver.wait(1000);
silver.revive(wallId);
```
