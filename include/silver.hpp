#ifndef SILVER_HPP
#define SILVER_HPP
#include <variant>

#include <map>

#include <string>

#include <iterator>

#include <condition_variable>

#include <memory>

#include <thread>

#include <functional>

#include <locale>

#include <numeric>

#include <queue>
#include <unordered_map>

#include <random>

constexpr int all_numbers = -1;
constexpr int immediate = -1;
constexpr int infinity = -1;
constexpr int ui_c = 1048576;

class Vector3 {
public:
  int x, y, z;

  // Constructors
  constexpr Vector3(int x_ = 0, int y_ = 0, int z_ = 0) : x(x_), y(y_), z(z_) {}

  // Conversion to Vector2 (cuts z)
  operator class Vector2() const;

  // Arithmetic operators
  Vector3 operator+(const Vector3 &other) const {
    return {x + other.x, y + other.y, z + other.z};
  }

  Vector3 operator-(const Vector3 &other) const {
    return {x - other.x, y - other.y, z - other.z};
  }

  Vector3 operator*(int scalar) const {
    return {x * scalar, y * scalar, z * scalar};
  }

  Vector3 &operator+=(const Vector3 &other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  Vector3 &operator-=(const Vector3 &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }

  Vector3 &operator*=(int scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
  }

  // Comparison operators
  bool operator==(const Vector3 &other) const {
    return x == other.x && y == other.y && z == other.z;
  }

  bool operator!=(const Vector3 &other) const { return !(*this == other); }

  bool operator<(const Vector3 &other) const {
    return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
  }

  // Static methods for common directions
  static constexpr Vector3 up() { return {0, -1, 0}; }
  static constexpr Vector3 down() { return {0, 1, 0}; }
  static constexpr Vector3 left() { return {-1, 0, 0}; }
  static constexpr Vector3 right() { return {1, 0, 0}; }
  static constexpr Vector3 forward() { return {0, 0, 1}; }
  static constexpr Vector3 backward() { return {0, 0, -1}; }
  static constexpr Vector3 zero() { return {0, 0, 0}; }
};

class Vector2 {
public:
  int x, y;
  constexpr Vector2(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}

  Vector2(const Vector3 &vec3) : x(vec3.x), y(vec3.y) {}

  operator Vector3() const { return Vector3(x, y, 0); }

  // Arithmetic operators
  Vector2 operator+(const Vector2 &other) const {
    return {x + other.x, y + other.y};
  }

  Vector2 operator-(const Vector2 &other) const {
    return {x - other.x, y - other.y};
  }

  Vector2 operator*(int scalar) const { return {x * scalar, y * scalar}; }

  Vector2 &operator+=(const Vector2 &other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  Vector2 &operator-=(const Vector2 &other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  Vector2 &operator*=(int scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
  }

  bool operator==(const Vector2 &other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Vector2 &other) const { return !(*this == other); }

  bool operator<(const Vector2 &other) const {
    return std::tie(x, y) < std::tie(other.x, other.y);
  }

  static constexpr Vector2 up() { return {0, -1}; }
  static constexpr Vector2 down() { return {0, 1}; }
  static constexpr Vector2 left() { return {-1, 0}; }
  static constexpr Vector2 right() { return {1, 0}; }
  static constexpr Vector2 zero() { return {0, 0}; }
};

inline constexpr Vector3 markAsUI{1048576, 1048576, 0};

struct components {

  std::string signMessage;
  std::string readerKey;

  bool isFluid = false;
  double diffusionSpeed = 1.0;
  int maximumDistance = 5;
  bool preventFlowing = false;
  int fluidDepth = 0;
  int fluidRoot = -1;
  int fluidParent = -1;
  double drySpeed = 100.0;
  double pressure = 1.0;
  bool isDead = false;

  bool followPhysics = false;
  double mass = 1.0;
  double dragCoefficient = 1.225;
  double angularDrag = 0.01;
  Vector3 Velocity = {0, 0, 0};
  double angularVelocity = 0.0;
  double position = 0.0;
  double angularPosition = 0.0;
  double drag = 0.0;
  bool gravity = false;
  bool isKinematic = false;
  double destroyForce = -1.0;

  bool canCollide = false;

  bool isUI = false;

  std::string animationBuffer;
  int playingID = -1;
};

class mesh;
class prefab {
public:
  std::string name;
  std::string shape;
  std::map<std::string, int> intValues;
  std::map<std::string, std::string> stringValues;
  std::vector<std::string> tags;
  bool isTransparent;
  components comp;

  prefab(const std::string n = "", const std::string shp = "",
         bool transp = false)
      : name(n), shape(shp), isTransparent(transp) {}

  prefab(const mesh &msh);
};

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
  void translate2(Vector2 offset);
  void translate3(Vector3 offset);
  Vector3 getPos2();
  Vector3 getPos3();
};

using World = std::map<int, mesh>;

struct zone {
public:
  std::variant<std::tuple<int, int, int, int>,
               std::vector<std::tuple<int, int, int, int>>>
      shape;

  zone(std::variant<std::tuple<int, int, int, int>,
                    std::vector<std::tuple<int, int, int, int>>>
           shape) {}
};

namespace Silver {
namespace {
struct ThreadInfo {
  std::thread t;
  bool paused = false;
  bool stop = false;
  std::condition_variable cv;
  std::mutex mtx;

  ThreadInfo() = default;
  ThreadInfo(const ThreadInfo &) = delete;
  ThreadInfo &operator=(const ThreadInfo &) = delete;
};
extern std::map<std::string, std::shared_ptr<ThreadInfo>> threads;

extern std::mutex thread_mutex;
extern std::vector<std::thread> activeThreads;
}; 
extern Vector2 WorldRangeStart, WorldRangeEnd;
bool isInZone(const zone &z, Vector3 pos);
namespace UI {

void changeToUI(std::variant<std::vector<int>, int> workspaceIDs);
int placeUI(const std::string objectName, int number, Vector3 pos);
}; 

namespace Fluid {

void Liquify(const std::variant<int, std::vector<int>> &IDs,
             double diffusionSpeed, int maximumDistance);
void Unliquify(const std::variant<int, std::vector<int>> &IDs);
void Unsolidify(const std::variant<int, std::vector<int>> &IDs);
void Solidify(const std::variant<int, std::vector<int>> &IDs);
}; // namespace Fluid
std::vector<int> duplicate(const std::variant<int, std::vector<int>> &IDs);
namespace Animation {

void applyAnimationRaw(std::variant<int, std::vector<int>> objID,
                       std::vector<std::string> animation, float fps,
                       int transition);
void applyAnimation(std::variant<int, std::vector<int>> objID,
                    const std::string filename);
void stopAnimation(std::variant<int, std::vector<int>> objID, bool reset);
}; // namespace Animation

namespace Particle {

void applyParticleComponent(int objID, const std::string objectName,
                            Vector3 pos, int radius, int lifeTime, double speed,
                            int quantity, int particleLifetime);
};
class Physics {};
void saveChunk(Vector3 range1, Vector3 range2, const std::string fileName,
               const std::string mode);
namespace Drawing {

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
}; // namespace Drawing
mesh *getMesh(int objID);
zone buildZone(std::variant<std::tuple<int, int, int, int>,
                            std::vector<std::tuple<int, int, int, int>>>
                   shape);
void setObjectPosition(const std::variant<int, std::vector<int>> objectID,
                       Vector3 pos);
void glideObjectPositionToTarget(
    const std::variant<int, std::vector<int>> objectIDs, int spriteID,
    float speed);
void setObjectPositionToTarget(
    const std::variant<int, std::vector<int>> objectIDs, int spriteID);
void sprayRectangle(int spawns, Vector3 center, Vector3 scale, std::string name,
                    int number);
void sprayOval(int spawns, Vector3 center, Vector3 scale, std::string name,
               int number);
void spray(int spawns, Vector3 center, int range, std::string name, int number);
bool isAlive(int obj);
void sprayLine(int spawns, Vector3 start, Vector3 end, std::string name,
               int number);
int placeObject(const std::string objectName, int number, Vector3 pos);

void createPrefab(const std::string name, const std::string shape);

void moveObjectXY(const std::variant<int, std::vector<int>> objectID,
                  Vector2 pos);
void moveObjectX(const std::variant<int, std::vector<int>> objectID,
                 int x_offset);
void moveObjectY(const std::variant<int, std::vector<int>> objectID,
                 int y_offset);

void setObjectRandom(const std::variant<int, std::vector<int>> objectID,
                     const std::pair<int, int> &xRange,
                     const std::pair<int, int> &yRange);
void Rectangle(const std::string name, int number, Vector3 topLeft, int width,
               int height);

void RectangleHollow(const std::string name, int number, Vector3 topLeft,
                     int width, int height);

void CircleHollow(const std::string name, int number, Vector3 center,
                  int radius);
void Circle(const std::string name, int number, Vector3 center, int radius);
void debug(const std::string message, const std::string mode);
void Line(const std::string name, int number, Vector3 start, Vector3 end);

void Oval(const std::string name, int number, Vector3 center, Vector3 scale);
int getRandom(int min, int max);
void OvalHollow(const std::string name, int number, Vector3 center,
                Vector3 scale);
void setObjectX(const std::variant<int, std::vector<int>> objectID,
                Vector3 pos);
void setObjectY(const std::variant<int, std::vector<int>> objectID,
                Vector3 pos);
void setObjectXY(const std::variant<int, std::vector<int>> objectID,
                 Vector2 pos);
void applyFunction(const std::vector<int> &ids, std::function<void(int)> func,
                   char mode, ...);
void glideObjectRandom(const std::variant<int, std::vector<int>> &ids,
                       const std::pair<int, int> &xRange,
                       const std::pair<int, int> &yRange, float speed);
void moveObjectPosition(const std::variant<int, std::vector<int>> objectID,
                        Vector2 pos);
void glideObjectXY(const std::variant<int, std::vector<int>> &ids,
                   Vector2 offset, float speed, ...);

std::vector<Vector3> getRectanglePoints(Vector3 topLeft, int width, int height);
std::vector<Vector3> getRectangleHollowPoints(Vector3 topLeft, int width,
                                              int height);

std::vector<Vector3> getLinePoints(Vector3 start, Vector3 end);
std::vector<Vector3> getOvalPoints(Vector3 center, Vector3 scale);
std::vector<Vector3> getOvalHollowPoints(Vector3 center, Vector3 scale);

void removeScript(const std::string objectName,
                  const std::string scriptToRemove);
void glideObjectX(const std::variant<int, std::vector<int>> &ids, int x_offset,
                  long long speed, ...);
void glideObjectY(const std::variant<int, std::vector<int>> &ids, int y_offset,
                  long long speed, ...);

void setWorldBounds(Vector2 start, Vector2 end);
std::vector<int> seekTag(const std::string tag);
void applyTag(const std::vector<int> &ids, const std::string tag);
void setRawMode(bool value);

std::pair<int, int> getConsoleSize();
Vector2 getConsoleCenter();
void walk(const std::string name, std::variant<std::vector<int>, int> number,
          int steps, int direction);

namespace Mouse {
void stopVMouse();
void startVMouse(int l, int r, int u, int d, int c);
extern int mouseKey;
extern int cursorPositionX;
extern int cursorPositionY;
extern std::string mouseIcon;
extern bool hideMouse;
bool wasMouseClicked();

}; // namespace Mouse

std::vector<std::string> getTag(int id);
void setConsoleTitle(const std::string title);
namespace Camera {

extern std::string null_rep;
extern std::vector<std::string> topText;
extern std::vector<std::string> rightText;
extern std::vector<std::string> leftText;
extern std::vector<std::string> bottomText;
extern bool sideLimit;
extern bool topDownLimit;
extern bool printSpaces;

extern int topAlign, bottomAlign, leftAlign, rightAlign;
extern bool markOutOfBounds;
extern std::string out_rep;
extern bool cutOutOfBounds;

extern Vector2 CameraConsolePos;
extern int anchor;

extern Vector3 CameraPos;
extern Vector3 CameraScale;
extern int cameraRotation;
extern bool isFlippedHorizontal, isFlippedVertical;
extern int cellSize;
extern bool isFirst;
extern std::mutex bufferMutex;
extern bool gridMode;
extern bool fillMode;
extern bool hideMouse;
extern std::map<std::tuple<int, int>, std::string> lastFrame;
void setCam2(Vector2 pos, Vector2 scale);
void setCam3(Vector3 pos, Vector3 scale);

void printCam();
void flipCamera(bool x, bool y);
Vector2 getScreenPosition(Vector3 pos);
void setCameraFlip(bool x, bool y);
void pivotCamera(int angle);
void addPivotCamera(int angle);
void shakeCameraOnce(float intensity);
void shakeCamera(float intensity, int shakes, float delayBetweenShakes);
void zoomCamera(Vector3 V);
void addCameraDepth(int X);
void cell(int c);
void configCameraException(const std::string n);
void setCameraDepth(int X);
void moveCamera(Vector3 V);
void startVideo(float FPS);
void endVideo();
void restartVideo(float FPS);
void photo();

std::vector<std::vector<std::string>> gPhoto();
}; // namespace Camera
void kill(std::variant<int, std::vector<int>> objIDs);
void revive(std::variant<int, std::vector<int>> objIDs);
void destroy(std::variant<int, std::vector<int>> objIDs);
void gotoxy(int x, int y);
int addObject(const std::string objectName, Vector3 pos);
int put(const std::string objectName, Vector3 pos);
int unoccupiedNumber(const std::string objectName);
void createEmptyPrefab(const std::string name);

std::vector<int> getObjectsAt3(Vector3 pos);
std::vector<int> getObjectsAt2(Vector2 pos);

namespace Threading {

void tRun(std::function<void()> func);

void dRun(std::function<void()> func, float time);

void createThread(const std::string name, std::function<void()> func);
void destroyThread(const std::string name);

void stopAllThreads();

void pauseThread(const std::string name);

void resumeThread(const std::string name);
}; // namespace Threading

namespace Timer {
void update(const std::string name);

float getDeltaTime(const std::string name);

void startTimer(const std::string name);

void stopTimer(const std::string name);

void resetTimer(const std::string name);

long long getDuration(const std::string name);

extern std::map<std::string, bool> running;
extern std::map<std::string, long long> duration;
extern std::map<std::string, std::chrono::high_resolution_clock::time_point>
    startTime;
extern std::map<std::string, std::chrono::high_resolution_clock::time_point>
    lastTime;
extern std::map<std::string, float> deltaTime;
}; // namespace Timer

namespace Scene {
extern std::map<std::string, World> savedScenes;
void saveCurrentWorld(const std::string name);
void loadScene(const std::string name);
void resetScene();
}; // namespace Scene

void clear();
void clean(Vector3 first, Vector3 second);
void loadChunk(const std::string file);
void wait(int time);

mesh getMeshValue(int objID);

Vector3 getLocation(int id);

double calculateDistance2(Vector2 first, Vector2 second);
double calculateDistance3(Vector3 first, Vector3 second);

void hold();
std::vector<int> all();
std::vector<int> findObjects(const std::string name,
                             std::variant<std::vector<int>, int> number);
std::vector<mesh> getMeshValuesAt3(Vector3 pos);
std::vector<mesh> getMeshValuesAt2(Vector2 pos);

namespace Keyboard {

// Constants for frequently used keys
extern const int upKey, downKey, leftKey, rightKey, escapeKey;

// Function prototypes
char getKey();                          // Retrieves the most recently pressed key
bool isKey(int key);                     // Checks if a key is the most recently pressed key
bool isKeyDown(int key);                 // Checks if a key is currently pressed
bool isKeyUp(int key);                   // Checks if a key is currently released
extern int holdThreshold ;

extern char keyBuffer;                   // Holds the most recently pressed key
extern bool caseSensitive;               // Flag to indicate if case-sensitivity is enabled

}; // namespace Keyboard


}; // namespace Silver
void startLoading();
extern World workspace;

extern World killedSprites;
extern const World emptyWorld;
extern std::map<const std::string, prefab> prefabrications;

#endif
