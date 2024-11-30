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

class Vec3 {
public:
  int x, y, z;

  Vec3 operator+(const Vec3 &other) const {
    return Vec3(x + other.x, y + other.y, z + other.z);
  }

  Vec3 operator-(const Vec3 &other) const {
    return Vec3(x - other.x, y - other.y, z - other.z);
  }

  Vec3 operator*(int scalar) const {
    return Vec3(x * scalar, y * scalar, z * scalar);
  }

  Vec3 &operator+=(const Vec3 &other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  Vec3 &operator-=(const Vec3 &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }

  Vec3 &operator*=(int scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
  }

  bool operator==(const Vec3 &other) const {
    return x == other.x && y == other.y && z == other.z;
  }

  bool operator!=(const Vec3 &other) const { return !(*this == other); }

  bool operator<(const Vec3 &other) const {
    return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
  }

  static const Vec3 up() { return Vec3(0, 1, 0); }
  static const Vec3 down() { return Vec3(0, -1, 0); }
  static const Vec3 left() { return Vec3(-1, 0, 0); }
  static const Vec3 right() { return Vec3(1, 0, 0); }
  static const Vec3 forward() { return Vec3(0, 0, 1); }
  static const Vec3 backward() { return Vec3(0, 0, -1); }
  static const Vec3 zero() { return Vec3(0, 0, 0); }
  constexpr Vec3(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}
  constexpr Vec3() : x(0), y(0), z(0) {}
};

inline constexpr Vec3 markAsUI{1048576, 1048576, 0};

class Vec2 {
public:
  int x, y;

  constexpr Vec2(int _x, int _y) : x(_x), y(_y) {}

  operator Vec3() const { return Vec3(x, y, 0); }

  Vec2 operator+(const Vec2 &other) const {
    return Vec2(x + other.x, y + other.y);
  }

  Vec2 operator-(const Vec2 &other) const {
    return Vec2(x - other.x, y - other.y);
  }

  Vec2 operator*(int scalar) const { return Vec2(x * scalar, y * scalar); }

  bool operator==(const Vec2 &other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Vec2 &other) const { return !(*this == other); }

  bool operator<(const Vec2 &other) const {
    return std::tie(x, y) < std::tie(other.x, other.y);
  }

  bool operator==(const Vec3 &other) const {
    return x == other.x && y == other.y && other.z == 0;
  }
};

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
  Vec3 Velocity = {0, 0, 0};
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
  Vec3 position;
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
  void translate2(Vec3 offset);
  void translate3(Vec3 offset);
  Vec3 getPos2();
  Vec3 getPos3();
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

class Silver {
private:
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

    std::map<std::string, std::shared_ptr<ThreadInfo>> threads;

    std::mutex thread_mutex;
    std::vector<std::thread> activeThreads;
public:
  Vec2 WorldRangeStart = {-25, -25}, WorldRangeEnd = {25, 25};
  bool isInZone(const zone &z, Vec3 pos);
  class UI {

    void changeToUI(std::variant<std::vector<int>, int> workspaceIDs);
    int placeUI(const std::string objectName, int number, Vec3 pos);
  };

  class Fluid {
  public:
    void Liquify(const std::variant<int, std::vector<int>> &IDs,
                 double diffusionSpeed, int maximumDistance);
    void Unliquify(const std::variant<int, std::vector<int>> &IDs);
    void Unsolidify(const std::variant<int, std::vector<int>> &IDs);
    void Solidify(const std::variant<int, std::vector<int>> &IDs);
  };
  std::vector<int> duplicate(const std::variant<int, std::vector<int>> &IDs);
  class Animation {
  public:
    void applyAnimationRaw(std::variant<int, std::vector<int>> objID,
                           std::vector<std::string> animation, float fps,
                           int transition);
    void applyAnimation(std::variant<int, std::vector<int>> objID,
                        const std::string filename);
    void stopAnimation(std::variant<int, std::vector<int>> objID, bool reset);
  };

  class Particle {
  public:
    void applyParticleComponent(int objID, const std::string objectName,
                                Vec3 pos, int radius, int lifeTime,
                                double speed, int quantity,
                                int particleLifetime);
  };
  class Physics {};
  void saveChunk(Vec3 range1, Vec3 range2, const std::string fileName,
                 const std::string mode);
  class Drawing {
  public:
    void draw(Vec3 pos, std::string c);
    void Line(Vec3 start, Vec3 end, std::string c);
    void Rectangle(Vec3 topLeft, int width, int height, std::string c);
    void Circle(Vec3 center, int radius, std::string c);
    void CircleHollow(Vec3 center, int radius, std::string c);
    void RectangleHollow(Vec3 topLeft, int width, int height, std::string c);
    void Oval(Vec3 center, int radiusX, int radiusY, std::string c);
    void OvalHollow(Vec3 center, int radiusX, int radiusY, std::string c);

    void sprayRectangle(int spawns, Vec3 center, Vec3 scale, std::string c);
    void sprayOval(int spawns, Vec3 center, Vec3 scale, std::string c);
    void spray(int spawns, Vec3 center, int range, std::string c);
    void sprayLine(int spawns, Vec3 start, Vec3 end, std::string c);
  };
  mesh *getMesh(int objID);
  zone buildZone(std::variant<std::tuple<int, int, int, int>,
                              std::vector<std::tuple<int, int, int, int>>>
                     shape);
  void setObjectPosition(const std::variant<int, std::vector<int>> objectID,
                         Vec3 pos);
  void glideObjectPositionToSprite(
      const std::variant<int, std::vector<int>> objectIDs, int spriteID,
      float speed);
  void
  setObjectPositionToSprite(const std::variant<int, std::vector<int>> objectIDs,
                            int spriteID);
  void sprayRectangle(int spawns, Vec3 center, Vec3 scale, std::string name,
                      int number);
  void sprayOval(int spawns, Vec3 center, Vec3 scale, std::string name,
                 int number);
  void spray(int spawns, Vec3 center, int range, std::string name, int number);
  bool isAlive(int obj);
  void sprayLine(int spawns, Vec3 start, Vec3 end, std::string name,
                 int number);
  int placeObject(const std::string objectName, int number, Vec3 pos);

  void createPrefab(const std::string name, const std::string shape);

  void moveObjectXY(const std::variant<int, std::vector<int>> objectID,
                    Vec3 pos);
  void moveObjectX(const std::variant<int, std::vector<int>> objectID,
                   int x_offset);
  void moveObjectY(const std::variant<int, std::vector<int>> objectID,
                   int y_offset);

  void setObjectRandom(const std::variant<int, std::vector<int>> objectID,
                       const std::pair<int, int> &xRange,
                       const std::pair<int, int> &yRange);
  void Rectangle(const std::string name, int number, Vec3 topLeft, int width,
                 int height);

  void RectangleHollow(const std::string name, int number, Vec3 topLeft,
                       int width, int height);

  void CircleHollow(const std::string name, int number, Vec3 center,
                    int radius);
  void Circle(const std::string name, int number, Vec3 center, int radius);
  void debug(const std::string message, const std::string mode);
  void Line(const std::string name, int number, Vec3 start, Vec3 end);

  void Oval(const std::string name, int number, Vec3 center, Vec3 scale);
  int getRandom(int min, int max);
  void OvalHollow(const std::string name, int number, Vec3 center, Vec3 scale);
  void setObjectX(const std::variant<int, std::vector<int>> objectID, Vec3 pos);
  void setObjectY(const std::variant<int, std::vector<int>> objectID, Vec3 pos);
  void setObjectXY(const std::variant<int, std::vector<int>> objectID,
                   Vec3 pos);
  void applyFunction(const std::vector<int> &ids,
                                      std::function<void(int)> func, char mode,
                                      ...);
  void glideObjectRandom(const std::variant<int, std::vector<int>> &ids,
                         const std::pair<int, int> &xRange,
                         const std::pair<int, int> &yRange, float speed);
  void moveObjectPosition(const std::variant<int, std::vector<int>> objectID,
                          Vec3 pos);
  void glideObjectXY(const std::variant<int, std::vector<int>> &ids,
                     const Vec3 &offset, float speed, ...);

  std::vector<Vec3> getRectanglePoints(Vec3 topLeft, int width, int height);
  std::vector<Vec3> getRectangleHollowPoints(Vec3 topLeft, int width,
                                             int height);

  std::vector<Vec3> getLinePoints(Vec3 start, Vec3 end);
  std::vector<Vec3> getOvalPoints(Vec3 center, Vec3 scale);
  std::vector<Vec3> getOvalHollowPoints(Vec3 center, Vec3 scale);

  void removeScript(const std::string objectName,
                    const std::string scriptToRemove);
  void glideObjectX(const std::variant<int, std::vector<int>> &ids,
                    int x_offset, float speed, ...);
  void glideObjectY(const std::variant<int, std::vector<int>> &ids,
                    int y_offset, float speed, ...);

  void setWorldBounds(Vec2 start, Vec2 end);
  std::vector<int> seekTag(const std::string tag);
  void applyTag(const std::vector<int> &ids, const std::string tag);
  void setRawMode();
  void resetMode();
  std::pair<int, int> getConsoleSize();
  Vec2 getConsoleCenter();
  void walk(const std::string name, std::variant<std::vector<int>, int> number,
            int steps, int direction);
  class Mouse {
  public:
    void stopVMouse();
    void startVMouse(int l, int r, int u, int d, int c);
    int mouseKey;
    int cursorPositionX = 0;
    int cursorPositionY = 0;
    std::string mouseIcon = "🖱️";
    bool isMouse();
  };

  std::vector<std::string> getTag(int id);
  void setConsoleTitle(const std::string title);
  class Camera {
  public:
    std::string null_rep = "🟫";
    std::vector<std::string> topText = {};
    std::vector<std::string> rightText = {};
    std::vector<std::string> leftText = {};
    std::vector<std::string> bottomText = {};
    bool sideLimit = false;
    bool topDownLimit = true;
    bool printSpaces = true;
    bool absoluteAlign = true;
    int topAlign = 1, bottomAlign = 0, leftAlign = 0, rightAlign = 0;
    bool markOutOfBounds = false;
    std::string out_rep = "🟦️";
    bool cutOutOfBounds = false;

    std::string mouseIcon;
    Vec2 CameraConsolePos = {0, 0};
    int anchor = 0;

    Vec3 CameraPos = {0, 0, 0};
    Vec3 CameraScale = {0, 0, 0};
    int cameraRotation = 0;
    int isFlippedHorizonal = 1, isFlippedVertical = 1;
    int cellSize = 2;
    bool isFirst = true;
    std::mutex bufferMutex;
    bool gridMode = true;
    bool fillMode = true;
    bool hideMouse = true;
    std::map<std::tuple<int, int>, std::string> lastFrame;
    void setCam2(Vec3 pos, Vec3 scale);
    void setCam3(Vec3 pos, Vec3 scale);

    void printCam();
    void flipCamera(int X, int Y);
    Vec2 getScreenPosition(Vec3 pos);
    void SetCameraFlip(int X, int Y);
    void pivotCamera(int angle);
    void addPivotCamera(int angle);
    void shakeCameraOnce(float intensity);
    void shakeCamera(float intensity, int shakes, float delayBetweenShakes);
    void zoomCamera(Vec3 V);
    void addCameraDepth(int X);
    void cell(int c);
    void configCameraException(const std::string n);
    void setCameraDepth(int X);
    void moveCamera(Vec3 V);
    void startVideo(int FPS);
    void endVideo();
    void restartVideo(int FPS);
    void photo();

    std::vector<std::vector<std::string>> gPhoto();
  };
  void kill(std::variant<int, std::vector<int>> objIDs);
  void revive(std::variant<int, std::vector<int>> objIDs);
  void destroy(std::variant<int, std::vector<int>> objIDs);
  void gotoxy(int x, int y);
  int addObject(const std::string objectName, Vec3 pos);
  int put(const std::string objectName, Vec3 pos);
  int unoccupiedNumber(const std::string objectName);
  void createEmptyObject(const std::string name);

  std::vector<int> spriteAt3(Vec3 pos);
  std::vector<int> spriteAt2(Vec2 pos);



  class Threading {
  private:
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

    std::map<std::string, std::shared_ptr<ThreadInfo>> threads;

    std::mutex thread_mutex;
    std::vector<std::thread> activeThreads;

  public:
    void tRun(std::function<void()> func);

    void dRun(std::function<void()> func, float time);


    void createThread(const std::string name, std::function<void()> func);
    void destroyThread(const std::string name);

    void stopAllThreads();

    void pauseThread(const std::string name);

    void resumeThread(const std::string name);
  };

  class Timer {
  public:
    void update(const std::string name);

    float getDeltaTime(const std::string name) const;

    void startTimer(const std::string name);

    void stopTimer(const std::string name);

    void resetTimer(const std::string name);

    long long getDuration(const std::string name) const;

  private:
    std::map<std::string, bool> running;
    std::map<std::string, long long> duration;
    std::map<std::string, std::chrono::high_resolution_clock::time_point>
        startTime;
    std::map<std::string, std::chrono::high_resolution_clock::time_point>
        lastTime;
    std::map<std::string, float> deltaTime;
  };
  class Scene {
  public:
    std::map<std::string, World> scenes;
    void saveWorldAs(const std::string name);
    void changeSceneTo(const std::string name);
    void resetScene();
  };
  void clear();
  void clean(Vec3 first, Vec3 second);
  void loadChunk(const std::string file);
  void wait(float time);

  mesh getMeshValue(int objID);

  Vec3 getLocation(int id);
  std::vector<int>
  getLocationAxis(const std::variant<int, std::vector<int>> objectID,
                  char axis);
  std::vector<int>
  getLocationX(const std::variant<int, std::vector<int>> objectID);
  std::vector<int>
  getLocationY(const std::variant<int, std::vector<int>> objectID);
  std::vector<int>
  getLocationZ(const std::variant<int, std::vector<int>> objectID);

  void hold();
  std::vector<int> all();
  std::vector<int> findObjects(const std::string name,
                               std::variant<std::vector<int>, int> number);
class Keyboard {
public:
    const int upKey = 65, downKey = 66, leftKey = 68, rightKey = 67,
              escapeKey = 27;

    std::map<char, int> keys;
    char keyBuffer;
    char getKey();
    bool isKey(char Key);

    void stopVMouse();
    void startVMouse(int l, int r, int u, int d, int c);
    int mouseKey;
    int cursorPositionX = 0;
    int cursorPositionY = 0;
    std::string mouseIcon = "🖱️";
    bool isMouse();
  };

  Mouse mouse;
  Keyboard keyboard;
  Timer timer;
  Threading threading;
  
  Camera camera;
  Drawing drawing;
  Animation animation;
  Fluid fluid;
  Particle particle;
  Physics physics;
  UI ui;

  int sprites_count = 0;
};

void startLoading();
extern World workspace;

extern World killedSprites;
extern const World emptyWorld;
extern std::map<const std::string, prefab> prefabrications;
extern Silver silver;
extern Vec3 Vector3;

#endif
