# Drawing functions 

## Function Prototypes
```
void draw(Vector3 pos, std::string c);
void Line(Vector3 start, Vector3 end, std::string c);
void Rectangle(Vector3 topLeft, int width, int height, std::string c);
void Circle(Vector3 center, int radius, std::string c);
void CircleHollow(Vector3 center, int radius, std::string c);
void RectangleHollow(Vector3 topLeft, int width, int height, std::string c);
void Oval(Vector3 center, int radiusX, int radiusY, std::string c);
void OvalHollow(Vector3 center, int radiusX, int radiusY, std::string c);

void sprayRectangle(int spawns, Vector3 center, Vector3 scale, std::string c);
void sprayOval(int spawns, Vector3 center, Vector3 scale, std::string c);
void spray(int spawns, Vector3 center, int range, std::string c);
void sprayLine(int spawns, Vector3 start, Vector3 end, std::string c);
```

## Description
| **Function**                        | **Description**                                                                                                                                       |
|-------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------|
| `void draw(Vector3 pos, std::string c)`             | Draws a single point at the given position (`pos`) with the specified string (`c`) representing the content to be drawn.                            |
| `void Line(Vector3 start, Vector3 end, std::string c)` | Draws a line between two given points (`start` and `end`) with the specified string (`c`) representing the content to be drawn.                      |
| `void Rectangle(Vector3 topLeft, int width, int height, std::string c)` | Draws a filled rectangle with the given top-left corner (`topLeft`), width (`width`), and height (`height`) using the string (`c`).               |
| `void Circle(Vector3 center, int radius, std::string c)` | Draws a filled circle with the given center (`center`) and radius (`radius`) using the string (`c`).                                                |
| `void CircleHollow(Vector3 center, int radius, std::string c)` | Draws a hollow circle with the given center (`center`) and radius (`radius`) using the string (`c`).                                                |
| `void RectangleHollow(Vector3 topLeft, int width, int height, std::string c)` | Draws a hollow rectangle with the given top-left corner (`topLeft`), width (`width`), and height (`height`) using the string (`c`).              |
| `void Oval(Vector3 center, int radiusX, int radiusY, std::string c)` | Draws a filled oval with the given center (`center`), horizontal radius (`radiusX`), and vertical radius (`radiusY`) using the string (`c`).     |
| `void OvalHollow(Vector3 center, int radiusX, int radiusY, std::string c)` | Draws a hollow oval with the given center (`center`), horizontal radius (`radiusX`), and vertical radius (`radiusY`) using the string (`c`).    |
| `void sprayRectangle(int spawns, Vector3 center, Vector3 scale, std::string c)` | Places multiple spawns (`spawns`) within a rectangular shape defined by the center (`center`) and scale (`scale`), using the string (`c`).      |
| `void sprayOval(int spawns, Vector3 center, Vector3 scale, std::string c)` | Places multiple spawns (`spawns`) within an oval shape defined by the center (`center`) and scale (`scale`), using the string (`c`).            |
| `void spray(int spawns, Vector3 center, int range, std::string c)` | Places multiple spawns (`spawns`) within a circular area around a center (`center`) with a specified range (`range`), using the string (`c`).     |
| `void sprayLine(int spawns, Vector3 start, Vector3 end, std::string c)` | Places multiple spawns (`spawns`) along a line between two points (`start` and `end`), using the string (`c`).                                   |

