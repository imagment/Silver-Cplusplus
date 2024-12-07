# Camera Movements

## Table of Contents
- [Function prototypes](#function-prototypes)
- [Details](#details)
  
## Function prototypes
```cpp
void setCam2(Vector2 pos, Vector2 scale); // Sets camera's scale and position (2D)
void setCam3(Vector3 pos, Vector3 scale); // Sets camera's scale and position (3D)
void setCameraFlip(bool x, bool y); // true: Flipped, false: Normal
void flipCamera(bool x, bool y); // true: Toggle, false: Leave it
void pivotCamera(int angle); // Set camera's rotation
void addPivotCamera(int angle); // Adds to the camera's current rotation
void shakeCamera(float intensity); //Shakes the camera
void zoomCamera(Vector3 v); // Changes the camera scale
void moveCamera(Vector3 v); // Moves the camera in a certain offset
```


## Details

| **Function**                  | **Description**                                                                                                                                       | **Parameters**                                                                                                   | **Usage Example**                                                                                                                                                 |
|-------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `void setCam2(Vector2 pos, Vector2 scale);` | Sets the camera's position and scale for 2D.                                                                                                  | `pos`: Position in 2D (e.g., `Vector2(x, y)`).<br>`scale`: Scale in 2D (e.g., `Vector2(sx, sy)`).               | `setCam2(Vector2(100, 200), Vector2(1.5, 1.5));` // Moves camera to (100, 200) and scales view by 1.5x.                                                           |
| `void setCam3(Vector3 pos, Vector3 scale);` | Sets the camera's position and scale for 3D.                                                                                                  | `pos`: Position in 3D (e.g., `Vector3(x, y, z)`).<br>`scale`: Scale in 3D (e.g., `Vector3(sx, sy, sz)`).        | `setCam3(Vector3(10, 20, 30), Vector3(2, 2, 2));` // Moves camera to (10, 20, 30) in 3D space and doubles the scale.                                              |
| `void setCameraFlip(bool x, bool y);`       | Sets whether the camera's view is flipped horizontally and/or vertically.                                                                     | `x`: Flip on X-axis (true/false).<br>`y`: Flip on Y-axis (true/false).                                           | `setCameraFlip(true, false);` // Flips the camera horizontally but not vertically.                                                                               |
| `void flipCamera(bool x, bool y);`          | Toggles flipping of the camera on the X and/or Y axis. If `true`, the corresponding axis is flipped; otherwise, it remains as-is.             | `x`: Toggle flip on X-axis (true/false).<br>`y`: Toggle flip on Y-axis (true/false).                             | `flipCamera(true, false);` // Toggles horizontal flipping without affecting vertical flipping.                                                                   |
| `void pivotCamera(int angle);`             | Sets the camera's rotation (pivot) to a specific angle in degrees.                                                                            | `angle`: Rotation angle in degrees.                                                                             | `pivotCamera(45);` // Rotates the camera to 45 degrees.                                                                                                          |
| `void addPivotCamera(int angle);`          | Adds to the camera's current rotation.                                                                                                        | `angle`: Additional rotation angle in degrees.                                                                  | `addPivotCamera(15);` // Adds 15 degrees to the current camera rotation.                                                                                         |
| `void shakeCamera(float intensity);`       | Simulates a shaking effect with a given intensity, useful for dramatic effects like explosions or impacts.                                    | `intensity`: Strength of the shake effect (e.g., 0.5 for mild shake, 2.0 for intense shake).                     | `shakeCamera(1.0);` // Applies a medium intensity camera shake.                                                                                                  |
| `void zoomCamera(Vector3 v);`              | Adjusts the camera's scale by a given vector, effectively zooming in or out.                                                                 | `v`: Change in scale (e.g., `Vector3(dx, dy, dz)`).                                                             | `zoomCamera(Vector3(0.5, 0.5, 0));` // Zooms the camera in by reducing X and Y scale by 0.5 each.                                                                |
| `void moveCamera(Vector3 v);`              | Moves the camera by a certain offset from its current position.                                                                               | `v`: Offset vector (e.g., `Vector3(dx, dy, dz)`).                                                               | `moveCamera(Vector3(10, -5, 0));` // Moves the camera 10 units right and 5 units down.                                                                           |
