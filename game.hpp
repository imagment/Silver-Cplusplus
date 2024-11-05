#ifndef GAME_H
#define GAME_H

#include <bits/stdc++.h>

#include <map>

#include <string>

#include <iterator>

#include <condition_variable>

#include "game.hpp"

#include <memory>

#include <locale>

#include <numeric>

#include <unordered_map>

#define MAX_TOKEN_SIZE 1000

#define all - 1
#define origin - 2
#define immediate - 1
#define upKey 65
#define downKey 66
#define leftKey 68
#define rightKey 67
#define escapeKey 27
#define drawing - 214743648
#define pointer "OBJECT_POINTER"
#define World map < int, mesh >
  using namespace std;

class Vec3 {
  public: int x,
  y,
  z;

  Vec3(int _x, int _y, int _z): x(_x),
  y(_y),
  z(_z) {}

  Vec3 operator + (const Vec3 & other) const {
    return Vec3(x + other.x, y + other.y, z + other.z);
  }

  Vec3 operator - (const Vec3 & other) const {
    return Vec3(x - other.x, y - other.y, z - other.z);
  }

  Vec3 operator * (int scalar) const {
    return Vec3(x * scalar, y * scalar, z * scalar);
  }

  Vec3 & operator += (const Vec3 & other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return * this;
  }

  Vec3 & operator -= (const Vec3 & other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return * this;
  }

  Vec3 & operator *= (int scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return * this;
  }

  bool operator == (const Vec3 & other) const {
    return x == other.x && y == other.y && z == other.z;
  }

  bool operator != (const Vec3 & other) const {
    return !( * this == other);
  }

  bool operator < (const Vec3 & other) const {
    return tie(x, y, z) < tie(other.x, other.y, other.z);
  }

  static
  const Vec3 up() {
    return Vec3(0, 1, 0);
  }
  static
  const Vec3 down() {
    return Vec3(0, -1, 0);
  }
  static
  const Vec3 left() {
    return Vec3(-1, 0, 0);
  }
  static
  const Vec3 right() {
    return Vec3(1, 0, 0);
  }
  static
  const Vec3 forward() {
    return Vec3(0, 0, 1);
  }
  static
  const Vec3 backward() {
    return Vec3(0, 0, -1);
  }
  static
  const Vec3 zero() {
    return Vec3(0, 0, 0);
  }
};

class Vec2 {
  public: int x,
  y;

  Vec2(int _x, int _y): x(_x),
  y(_y) {}

  operator Vec3() const {
    return Vec3(x, y, 0);
  }

  Vec2 operator + (const Vec2 & other) const {
    return Vec2(x + other.x, y + other.y);
  }

  Vec2 operator - (const Vec2 & other) const {
    return Vec2(x - other.x, y - other.y);
  }

  Vec2 operator * (int scalar) const {
    return Vec2(x * scalar, y * scalar);
  }

  bool operator == (const Vec2 & other) const {
    return x == other.x && y == other.y;
  }

  bool operator != (const Vec2 & other) const {
    return !( * this == other);
  }

  bool operator < (const Vec2 & other) const {
    return tie(x, y) < tie(other.x, other.y);
  }

  bool operator == (const Vec3 & other) const {
    return x == other.x && y == other.y && other.z == 0;
  }
};

struct sprite {
  Vec3 position;
  string name;
  int number;

  map < string, int > intValues;
  map < string, string > stringValues;

  vector < string > tags;

  sprite(const Vec3 & pos = Vec3(0, 0, 0),
    const string & n = "",
      int num = 0): position(pos), name(n), number(num) {}
};

struct components {
  public: string signMessage = "";
  string readerKey = "";

  bool isFluid = false;
  double diffusionSpeed = 1.f;
  int maximumDistance = 5;
  double pressure = 1;

  bool preventFlowing = false;
};

class mesh {
  public: sprite object;
  std::string scripts;
  std::string shape;

  string animationBuffer;
  int playingID = -1;
  int transparency = 0;
  components Components;

  mesh(): object(),
  scripts(""),
  shape(""),
  animationBuffer(""),
  transparency(0) {}
  mesh(sprite obj,
    const std::string & scr,
      const std::string & shp): object(obj),
  scripts(scr),
  shape(shp),
  transparency(0) {}

  // Parameterized constructor

  // Other member functions
  void setTransparency(int value) {
    transparency = (value == 0 || value == 1) ? value : transparency;
  }
  void translate2(Vec3 offset) {
    object.position.x += offset.x;
    object.position.y += offset.y;
  }
  void translate3(Vec3 offset) {
    object.position += offset;
  }

  Vec3 getPos2() {
    return {
      object.position.x,
      object.position.y,
      0
    };
  }
  Vec3 getPos3() {
    return object.position;
  }
};

struct objects {
  string name;

  string shape;
  vector < string > tags;

  int transparency;
  components Components;

};

struct scene {
  string name;
  vector < sprite > objects;
  vector < mesh > objectProps;
};

struct zone {
  public: variant < tuple < int,
  int,
  int,
  int > ,
  vector < tuple < int,
  int,
  int,
  int >>> shape;

  zone(variant < tuple < int, int, int, int > , vector < tuple < int, int, int, int >>> shape) {}
};

class Silver {

  public: bool isInZone(const zone & z, Vec3 pos);

  class Fluid {
    public: void Liquify(const variant < int, vector < int >> & IDs, double diffusionSpeed, int maximumDistance);
    void Unliquify(const variant < int, vector < int >> & IDs);
    void Unsolidify(const variant < int, vector < int >> & IDs);
    void Solidify(const variant < int, vector < int >> & IDs);

  };
  vector < int > duplicate(const variant < int, vector < int >> & IDs);
  class Animation {
    public: void applyAnimationRaw(variant < int, vector < int >> objID, vector < string > animation, float fps, int transition);
    void applyAnimation(variant < int, vector < int >> objID,
      const std::string & filename);
    void stopAnimation(variant < int, vector < int >> objID, bool reset);
  };
  class Drawing {
    public: void draw(Vec3 pos, string c);
    void Line(Vec3 start, Vec3 end, string c);
    void Rectangle(Vec3 topLeft, int width, int height, string c);
    void Circle(Vec3 center, int radius, string c);
    void CircleHollow(Vec3 center, int radius, string c);
    void RectangleHollow(Vec3 topLeft, int width, int height, string c);
    void Oval(Vec3 center, Vec3 scale, string c);
    void OvalHollow(Vec3 center, Vec3 scale, string c);

    void sprayRectangle(int spawns, Vec3 center, Vec3 scale, string c);
    void sprayOval(int spawns, Vec3 center, Vec3 scale, string c);
    void spray(int spawns, Vec3 center, int range, string c);
    void sprayLine(int spawns, Vec3 start, Vec3 end, string c);
  };
  mesh * GetObject(int objID);
  zone buildZone(variant < tuple < int, int, int, int > , vector < tuple < int, int, int, int >>> shape);
  void setObjectPosition(const variant < int, vector < int >> objectID, Vec3 pos);
  void glideObjectPositionToSprite(const variant < int, vector < int >> objectIDs, int spriteID, float speed);
  void setObjectPositionToSprite(const variant < int, vector < int >> objectIDs, int spriteID);
  void sprayRectangle(string name, int number, int spawns, Vec3 center, Vec3 scale);
  void sprayOval(string name, int number, int spawns, Vec3 center, Vec3 scale);
  void spray(string name, int number, int spawns, Vec3 center, int range);

  void createObject(const string name,
    const string shape);
  void sprayLine(int spawns, Vec3 start, Vec3 end, string c);
  int place(string objectName, int number, Vec3 pos);
  void sprayLine(string name, int number, int spawns, Vec3 start, Vec3 end);
  void moveObjectXY(const variant < int, vector < int >> objectID, Vec3 pos);
  void moveObjectX(const variant < int, vector < int >> objectID, int x_offset);
  void moveObjectY(const variant < int, vector < int >> objectID, int y_offset);

  void setObjectRandom(const variant < int, vector < int >> objectID,
    const pair < int, int > & xRange,
      const pair < int, int > & yRange);
  void Rectangle(string name, int number, Vec3 topLeft, int width, int height);

  void RectangleHollow(string name, int number, Vec3 topLeft, int width, int height);

  void CircleHollow(string name, int number, Vec3 center, int radius);
  void Circle(string name, int number, Vec3 center, int radius);

  void Line(string name, int number, Vec3 start, Vec3 end);

  void Oval(string name, int number, Vec3 center, Vec3 scale);
  int getRandom(int min, int max);
  void OvalHollow(string name, int number, Vec3 center, Vec3 scale);
  void setObjectX(const string & name,
    const variant < int, vector < int >> & number, int x);
  void setObjectY(const string & name,
    const variant < int, vector < int >> & number, int y);
  void setObjectXY(const variant < int, vector < int >> objectID, Vec3 pos);

  void glideObjectRandom(const variant < int, vector < int >> & ids,
    const pair < int, int > & xRange,
      const pair < int, int > & yRange, float speed);
  void moveObjectPosition(const variant < int, vector < int >> objectID, Vec3 pos);
  void glideObjectXY(const variant < int, vector < int >> & ids, Vec3 target_pos, float speed, ...);

  vector < Vec3 > getRectanglePoints(Vec3 topLeft, int width, int height);
  vector < Vec3 > getRectangleHollowPoints(Vec3 topLeft, int width, int height);

  vector < Vec3 > getLinePoints(Vec3 start, Vec3 end);
  vector < Vec3 > getOvalPoints(Vec3 center, Vec3 scale);
  vector < Vec3 > getOvalHollowPoints(Vec3 center, Vec3 scale);

  void removeScript(const string objectName,
    const string & scriptToRemove);
  void glideObjectX(const variant < int, vector < int >> & ids, int x_offset, float speed, ...);
  void glideObjectY(const variant < int, vector < int >> & ids, int y_offset, float speed, ...);

  void setWorldBounds(Vec3 world);
  vector < int > seekTag(const string & tag);
  void applyTag(const vector < int > & ids,
    const string & tag);
  void setRawMode();
  void resetMode();
  pair < int,
  int > getConsoleSize();
  Vec2 getConsoleCenter();
  void walk(string name, variant < vector < int > , int > number, int steps, int direction);
  class Mouse {
    public: void stopVMouse();
    void startVMouse(int l, int r, int u, int d, int c);
    int mouseKey;
    int cursorPositionX = 0;
    int cursorPositionY = 0;
    bool isMouse();
  };

  vector < string > getTag(int id);
  void setConsoleTitle(const std::string & title);
  class Camera {
    public: string topText,
    bottomText;
    string leftText;
    string rightText;
    void displayText(const string & text, int scaleX, int maxLeftWidth, bool isTop, int offsetX);
    int topAlign = 0,
    bottomAlign = 0,
    leftAlign = 0,
    rightAlign = 0;

    string mouseIcon;
    Vec3 CameraConsolePos = {
      0,
      0,
      0
    };
    int anchor = 0;
    /*
    0 1 2
    3 4 5
    6 7 8
    */

    Vec3 CameraPos = {
      0,
      0,
      0
    };
    Vec3 CameraScale = {
      0,
      0,
      0
    };
    int cameraRotation = 0;
    int isFlippedHorizonal = 1,
    isFlippedVertical = 1;
    int cellSize = 2;
    bool isFirst = true;
    mutex bufferMutex;
    bool gridMode = true;
    bool fillMode = true;
    bool hideMouse = true;
    map < tuple < int,
    int > ,
    string > lastFrame;
    void setCam3(Vec3 pos, Vec3 scale);
    void setCam2(Vec3 pos, Vec3 scale);
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
    void configCameraException(string o, string n);
    void setCameraDepth(int X);
    void moveCamera(Vec3 V);
    void startVideo(int FPS);
    void endVideo();
    void restartVideo(int FPS);
    void photo();
    vector < vector < string >> gPhoto();
  };

  void kill(const string & objectName,
    const variant < int, vector < int >> & number);
  void revive(const string & objectName,
    const variant < int, vector < int >> & number);

  void destroy(const string & objectName,
    const variant < int, vector < int >> & number);

  void gotoxy(int x, int y);
  int put(string objectName, Vec3 pos);

  vector < int > seek(string objectName);

  vector < int > spriteAt3(Vec3 pos);
  vector < int > spriteAt2(Vec3 pos);

  class Keyboard {
    public: map < char,
    int > keys;
    char keyBuffer;
    char getKey();
    bool isKey(char Key);
  };

  class Threading {
    private:

      struct ThreadInfo {
        thread t;
        bool paused = false;
        bool stop = false;
        condition_variable cv;
        mutex mtx;

        ThreadInfo() =
          default;
        ThreadInfo(const ThreadInfo & ) = delete;
        ThreadInfo & operator = (const ThreadInfo & ) = delete;

      };

    map < string,
    shared_ptr < ThreadInfo >> threads;

    mutex thread_mutex;
    vector < thread > activeThreads;

    public: void tRun(function < void() > func);

    void dRun(function < void() > func, float time);

    void applyFunction(const vector < int > & ids,
      function < void(int) > func, char mode, ...);
    void createThread(const string & name,
      function < void() > func);
    void destroyThread(const string & name);

    void stopAllThreads();

    void pauseThread(const string & name);

    void resumeThread(const string & name);
  };

  class Timer {
    public:

      void update(string name);

    float getDeltaTime(string name) const;

    void startTimer(string name);

    void stopTimer(string name);

    void resetTimer(string name);

    long long getDuration(string name) const;

    private: map < string,
    bool > running;
    map < string,
    long long > duration;
    map < string,
    chrono::high_resolution_clock::time_point > startTime;
    map < string,
    chrono::high_resolution_clock::time_point > lastTime;
    map < string,
    float > deltaTime;
  };
  class Scene {
    private: map < string,
    World > scenes;
    public: void saveWorldAs(string name);
    void changeSceneTo(string name);
    void resetScene();
  };
  void clear();
  void clean(Vec3 first, Vec3 second);
  void loadChunk(const string & file);
  void wait(float time);

  vector < mesh > MeshAt3(Vec3 pos);
  vector < mesh > MeshAt2(Vec3 pos);
  mesh GetMeshOf(int objID);

  Vec3 getLocation(int id);
  vector < int > getLocationAxis(const variant < int, vector < int >> objectID, char axis);
  vector < int > getLocationX(const variant < int, vector < int >> objectID);
  vector < int > getLocationY(const variant < int, vector < int >> objectID);
  vector < int > getLocationZ(const variant < int, vector < int >> objectID);
  int PlaceMesh(mesh m, Vec3 pos);
  void hold();
  vector < int > findObjects(string name, variant < vector < int > , int > number);
  Camera camera;
  Keyboard keyboard;
  Timer timer;
  Threading threading;
  Mouse mouse;
  Drawing draw;
  Animation animation;
  Fluid fluid;
  int sprites_count = 0;
};

void startLoading();

extern Silver silver;
extern Vec3 Vector3;
extern World workspace;
extern World killedSprites;
extern World empty;
extern map <
  const string, objects > prefabs;
#endif
