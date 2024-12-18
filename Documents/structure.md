# Structure

## Index

- [Design](#design)
  - [Workspace](#workspace)
  - [Prefabrication](#prefabrications)
  - [Killed Workspace](#killed-workspace)
  - [Structure Of Sprites](#structure-of-sprites)
  - [Structure Of Prefabrications](#structure-of-prefabs)

## Design
The workspace is the game's object map. All objects added using the put or place functions are stored in this map. 
The entry `workspace[k]` refers to the mesh in the workspace with the object ID k. <br>
Because the workspace is a map, you can perform various operations. <br> 

For example, 
```cpp
int player = Silver::put("player", Vec3(2,4,8)); // Places an object named 'player' and saves the object ID to variable 'player'
workspace.erase(player); // Removes an object with ID 'player' from the workspace.
// Alternative: Silver::destroy(player);
// However, note that `workspace.erase(player);` is faster than `Silver::destroy(player);`.
```


## Prefabrications
The prefabrication gets created when an object is created with the `createObject` function. Prefabrication can be on the `workspace` through functions like `place` or `put` <br>
Prefabrications are saved in the `prefabs` map. Their key is the name of the object. 

## Killed Workspace
Besides removing the object with the `destroy` function, there is another way to destroy an object. And that is the `kill` function. Unlike `destroy`, objects destroyed with `kill` can "revive". <br>
This means objects that are destroyed with the `kill` functions are temporarily removed from the `workspace` but can always return to the `workspace`. For example,

```cpp
int player = Silver::put("player", Vec3(2,4,8));
Silver::kill(player);
Silver::wait(1000); // Delay for 1 second to observe the change
Silver::revive(player);
```
This code removes the player from the workspace and returns it after 1 second. During this second, the object is saved on a map called `killedSprites`. <br>
like `workspace`, their key is the id of the object.

## Structure Of Sprites
All Sprites placed in the workspace have a class called 'mesh'. Here is a structure of mesh.
```cpp
class mesh {
public:
  Vector3 position;
  std::string name;
  std::string shape;
  int number;
  std::map<std::string, int> intValues;
  std::map<std::string, std::string> stringValues;
  std::vector<std::string> tags;
  bool isTransparent;
  components comp;

  mesh() : position(), number(0), isTransparent(false) {}

  mesh(const prefab &pfb)
      : position(), name(pfb.name), shape(pfb.shape), number(0),
        intValues(pfb.intValues), stringValues(pfb.stringValues),
        tags(pfb.tags), isTransparent(pfb.isTransparent), comp(pfb.comp) {}

  void setTransparence(bool value);
  void translate2(Vector3 offset);
  void translate3(Vector3 offset);
  Vector3 getPos2();
  Vector3 getPos3();
};
```
`position` indicates the position of the object. <br>
`name` indicates the name of the object. <br>
`number` indicates the number of the object. <br>
`shape` indicates the shape of the object. <br>
`intValues` is a map that saves an object's associated int values. <br>
`stringValues` is a map that saves an object's associated string values. <br>
`tags` is a map that saves an object's tags. <br>
`transparency` is an integer that saves an object's transparency. <br>
`mesh();` is a default constructer of mesh. <br>
<br>
<br>
You can use `workspace[objID]` to access the meshes on the workspace or use `getMeshValue` function from the silver class. <br>
Or instead of using functions like `createObject` and `place` to place objects in the workspace, you can build the structures of the mesh and place it directly <br>
by accessing the workspace. 

## Structure Of Prefabrications
All sprites created by functions like `createObject` has a class called `prefab`. Here is an implementation of it.
```
class prefab {
public:
    std::string name;
    std::string shape;
    std::map<std::string, int> intValues;
    std::map<std::string, std::string> stringValues;
    std::vector<std::string> tags;
    int transparency;
    components Components;

    prefab(const std::string& n = "", const std::string& shp = "", int transp = 0);
    prefab(const mesh& msh); 
};
```
`name` indicates the name of the object. <br>
`shape` indicates the shape of the object. <br>
`intValues` is a map that saves an object's associated int values. <br>
`stringValues` is a map that saves an object's associated string values. <br>
`tags` is a map that saves an object's tags. <br>
`transparency` is an integer that saves an object's transparency. <br>
`prefab(const std::string& n = "", const std::string& shp = "", int transp = 0)` is the default constructor of a prefab <br>
<br>
Unlike meshes, prefabs do not have numbers or positions. <br>
Also, note that prefabs are compatible with mesh. <br>

