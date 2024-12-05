# Function name : getMesh, getMeshValue

## Function prototype

```cpp
mesh * getMesh(int objID);
mesh getMeshValue(int objID);
```

## Implementations

```cpp
mesh * Silver::getMesh(int objID) {
  return & workspace[objID];
}

mesh Silver::getMeshValue(int objID) {
  return workspace[objID];
}
```

## Description
`getMesh` is used to bring a pointer of a mesh in the workspace. <br>
`getMeshValue` is used to bring a mesh directly from the workspace.

## Example Usage
```cpp
mesh player = Silver::getMeshValue(playerID);
Silver::getMesh(playerID)->intValues["lives"]=3;
```

## Alternative
Instead of `getMesh(objID)`, you can use `workspace[objID]`. 
