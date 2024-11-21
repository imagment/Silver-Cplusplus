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
because workspace is a map, you can do a variety of operations. <br> 

For example, 
```cpp
int player = silver.put("player", Vec3(2,4,8)); // Places a object named 'player' and saves the object ID to variable 'player'
workspace.erase(player); // Removes a object with ID 'player' from the workspace.
// Alternative: silver.destroy(player);
// However, note that `workspace.erase(player);` is faster than `silver.destroy(player);`.
```


## Prefabrications
When an object is created with the `createObject` function, the prefabrication get created. <br>
And a prefabrication can be on the `workspace` through functions like `place` or `put` <br>
Prefabrications are saved in `prefabs` map. Their key is the name of the object. 

## Killed Workspace
Besides removing the object with the `destroy` function, there is another way to destroy an object. <br>
And that is the `kill` function. Unlike `destroy`, object that are destroyed with `kill` can "revive". <br>
This means objects that are destroyed with the `kill` functions are temporary removed from the `workspace` <br>
but can always return to the `workspace`. For example,

```cpp
int player = silver.put("player", Vec3(2,4,8));
silver.kill(player);
silver.wait(1000); // Delay for 1 second to observe the change
silver.revive(player);
```
This code removes the player from the workspace, and brings it back after 1 second. <br>
During this second, the object is saved on a map called `killedSprites`. <br>
like `workspace`, their key is the id of the object.
