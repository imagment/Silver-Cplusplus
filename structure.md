> Warning: <br>
> This documentation is getting modified. <br>
> Thanks for your patience. <br>

# Structure

## Index

- [Design](#design)
  - [Workspace](#workspace)
  - [Prefabrication](#prefabrications)
  - [Killed Workspace](#killed-workspace)
  - [Sprites](#sprites)

## Design
The workspace is the game's object map. All objects added using the put or place functions are stored in this map. <br>
The entry `workspace[k]` refers to the mesh in the workspace with the object ID k. <br>
because the workspace is a map, you can do a variety of operations. <br> 

For example, 
```cpp
int player = silver.put("player", Vec3(2,4,8)); // Places an object named 'player' and saves the object ID to variable 'player'
workspace.erase(player); // Removes an object with ID 'player' from the workspace.
// Alternative: silver.destroy(player);
// However, note that `workspace.erase(player);` is faster than `silver.destroy(player);`.
```


## Prefabrications
The prefabrication gets created when an object is created with the `createObject` function. <br>
Prefabrication can be on the `workspace` through functions like `place` or `put` <br>
Prefabrications are saved in the `prefabs` map. Their key is the name of the object. 

## Killed Workspace
Besides removing the object with the `destroy` function, there is another way to destroy an object. <br>
And that is the `kill` function. Unlike `destroy`, objects destroyed with `kill` can "revive". <br>
This means objects that are destroyed with the `kill` functions are temporarily removed from the `workspace` <br>
but can always return to the `workspace`. For example,

```cpp
int player = silver.put("player", Vec3(2,4,8));
silver.kill(player);
silver.wait(1000); // Delay for 1 second to observe the change
silver.revive(player);
```
This code removes the player from the workspace and brings it back after 1 second. <br>
During this second, the object is saved on a map called `killedSprites`. <br>
like `workspace`, their key is the id of the object.

## Sprites
All Sprites have a class called 'mesh'. Here is a structure of mesh.
```cpp
class mesh {
public:
    Vec3 position;
    std::string name;
    std::string shape;
    int number;
    std::map<std::string, int> intValues;
    std::map<std::string, std::string> stringValues;
    std::vector<std::string> tags;
    int transparency;
    components Components;

    mesh();                      
    mesh(const prefab& pfb);  
    void setTransparency(int value);
    void translate2(Vec3 offset);
    void translate3(Vec3 offset);
    Vec3 getPos2() const;
    Vec3 getPos3() const;
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
You can use `workspace[objID]` to access the meshes on the workspace <br>
or use `getMeshValue` function from the silver class. <br>
Or instead of using functions like `createObject` and `place` to place objects <br>
in the workspace, you can build the structures of the mesh and place it directly <br>
by accessing the workspace. 
