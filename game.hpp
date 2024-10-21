#ifndef GAME_H
#define GAME_H

#include <bits/stdc++.h>
#include <map>
#include <string>
#include <iterator>
#include <condition_variable>
#include "game.hpp"
#include <memory>

#define MAX_TOKEN_SIZE 1000

#define all -1
#define origin -2
#define upKey 65
#define downKey 66
#define leftKey 68
#define rightKey 67
#define escapeKey 27
#define drawing -214743648
#define pointer "OBJECT_POINTER"
#define World multimap<tuple<int, int, int>, mesh> 
using namespace std;

class Vec3 {
public:
    int x, y, z;

    Vec3(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(int scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this; 
    }

    Vec3& operator-=(const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this; 
    }

    Vec3& operator*=(int scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this; 
    }

    bool operator==(const Vec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Vec3& other) const {
        return !(*this == other);
    }

    bool operator<(const Vec3& other) const {
        return tie(x, y, z) < tie(other.x, other.y, other.z);
    }

    static const Vec3 up() { return Vec3(0, 1, 0); }
    static const Vec3 down() { return Vec3(0, -1, 0); }
    static const Vec3 left() { return Vec3(-1, 0, 0); }
    static const Vec3 right() { return Vec3(1, 0, 0); }
    static const Vec3 forward() { return Vec3(0, 0, 1); }
    static const Vec3 backward() { return Vec3(0, 0, -1); }
    static const Vec3 zero() { return Vec3(0, 0, 0); }
};

class Vec2 {
public:
    int x, y;

    Vec2(int _x, int _y) : x(_x), y(_y) {}

    operator Vec3() const {
        return Vec3(x, y, 0);
    }

    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }

    Vec2 operator*(int scalar) const {
        return Vec2(x * scalar, y * scalar);
    }

    bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vec2& other) const {
        return !(*this == other);
    }

    bool operator<(const Vec2& other) const {
        return tie(x, y) < tie(other.x, other.y);
    }

    bool operator==(const Vec3& other) const {
        return x == other.x && y == other.y && other.z == 0;
    }
};

struct sprite {
    Vec3 position;
    string name;
    int number;

    map<string, int> intValues;
    map<string, string> stringValues;

    vector <string> tags;

    sprite(const Vec3& pos = Vec3(0,0,0), const string& n = "", int num = 0) 
        : position(pos), name(n), number(num) {}
};

struct animation {
    string animationName;  
    string animData;       
    int currentFrame;

    animation(const string& animName = "", const string& animDataVal = "", int frame = 0) 
        : animationName(animName), animData(animDataVal), currentFrame(frame) {}
};

struct components {
    string signMessage="";
    string readerKey="";
};

struct mesh {
    sprite object;               
    string scripts;
    string shape;
    int objectPointer = -1;
    animation* anim=nullptr;
    int transparency = 0; 
    components Components;  

};

struct objects {
    string name;
    string shape;
    vector <string> tags;
    string scripts;
    int transparency;
    animation* anim = nullptr;
    components Components;

};

struct scene {
    string name;
    vector<sprite> objects;
    vector<mesh> objectProps;
    vector<animation> animations;
};

struct zone {
public:
    variant<tuple <int,int,int,int>, vector<tuple<int,int,int,int>>> shape;

    zone(variant<tuple <int,int,int,int>, vector<tuple<int,int,int,int>>> shape) {}
};

extern multimap <tuple<int, int, int>, mesh> sprites;
extern map <tuple<string, int>, mesh> spritesName;
extern map <string, objects> globalObjects;
extern vector<scene> scenes;

class Silver {

public:
    bool isInZone(const zone& z, Vec3 pos);
    void drawOval(Vec3 center, Vec3 scale, string c);
    void drawOvalHollow(Vec3 center, Vec3 scale, string c);
    zone buildZone(variant<tuple<int,int,int,int>, vector<tuple<int,int,int,int>>> shape);
    void setObjectPositionToSprite(const string& name, const variant<int, vector<int>>& number,
                                       const string& target, int targetNumber);
    void sprayRectangle(int spawns, Vec3 center, Vec3 scale, string c);
    void sprayOval(int spawns, Vec3 center, Vec3 scale, string c);
    void spray(int spawns, Vec3 center, int range, string c);
    void sprayPlaceRectangle(string name, int number, int spawns, Vec3 center, Vec3 scale);
    void sprayPlaceOval(string name, int number, int spawns, Vec3 center, Vec3 scale);
    void sprayPlace(string name, int number, int spawns, Vec3 center, int range);
    void createObject(const string name, const string shape);
    void sprayLine(int spawns, Vec3 start, Vec3 end, string c);
    int place(string objectName, int number, Vec3 position);
    void sprayPlaceLine(string name, int number, int spawns, Vec3 start, Vec3 end);
    void moveObjectXY(const string& name, const variant<int, vector<int>>& number, Vec3 pos) ;
    void moveObjectX(const string& name, const variant<int, vector<int>>& number, int x_offset);
    void moveObjectY(const string& name, const variant<int, vector<int>>& number, int y_offset);

    void setObjectRandom(const string& name, const variant<int, vector<int>>& number);
    void placeRectangle(string name, int number, Vec3 topLeft, int width, int height);

    // Function to place a hollow rectangle
    void placeRectangleHollow(string name, int number, Vec3 topLeft, int width, int height);

    // Function to place a filled circle
    void placeCircle(string name, int number, Vec3 center, int radius);

    // Function to place a hollow circle
    void placeCircleHollow(string name, int number, Vec3 center, int radius);

    // Function to place a line between two points
    void placeLine(string name, int number, Vec3 start, Vec3 end);

    // Function to place a filled oval
    void placeOval(string name, int number, Vec3 center, Vec3 scale);

    // Function to place a hollow oval
    void placeOvalHollow(string name, int number, Vec3 center, Vec3 scale);
    void setObjectX(const string& name, const variant<int, vector<int>>& number, int x);
    void setObjectY(const string& name, const variant<int, vector<int>>& number, int y);
    void setObjectXY(const string& name, const variant<int, vector<int>>& number, Vec3 pos);
    void glideObjectRandom(const string& name, const variant<int, vector <int>>& number, float speed);
    void moveObjectPosition(const string& name, const variant<int, vector<int>>& number, Vec3 pos);
    void glideObjectXY(string name, const variant<vector<int>, int>& number, Vec3 target_pos, float speed, ...) ;

    void glideObjectPositionToSprite(const string& name, const variant<int, vector<int>>& number,
                                         const string& target, int targetNumber, float speed);
    void setObjectPosition(const string& name, const variant<int, vector<int>>& number, Vec3 pos);

    vector<Vec3> getRectanglePoints(Vec3 topLeft, int width, int height);
    vector<Vec3> getRectangleHollowPoints(Vec3 topLeft, int width, int height);

    vector<Vec3> getLinePoints(Vec3 start, Vec3 end);
    vector<Vec3> getOvalPoints(Vec3 center, Vec3 scale);
    vector<Vec3> getOvalHollowPoints(Vec3 center, Vec3 scale);

    void removeScript(const string objectName, const string& scriptToRemove);
    void glideObjectX(string name, const variant<int, vector<int>>& number, int x_offset, float speed);
    void glideObjectY(string name, const variant<int, vector<int>>& number, int y_offset, float speed);
    void applyComponent(const string object, int number, const string component, ...);
    void setWorldBounds(Vec3 world);
    vector<pair<string, int>> seekTag(const string& tag);

    void applyTag(const string& name, const variant<int, vector<int>>& number, const string& tag);
    void setRawMode();
    void resetMode();
    pair <int,int> getConsoleSize();
    Vec2 getConsoleCenter();
    void walk(string name, variant<vector<int>,int> number, int steps, int direction);
 class Mouse {
 public:
    void stopVMouse();
    void startVMouse(int l, int r, int u, int d, int c);
    int mouseKey;
    int cursorPositionX = 0;
    int cursorPositionY = 0;
    bool isMouse();
};

    vector<string> getTag(const string& name, int number);

class Camera {
  public:
    Vec3 CameraPos={0,0,0};
    Vec3 CameraScale={0,0,0};
    int cameraRotation=0;
    int isFlippedHorizonal=1, isFlippedVertical=1;
    int cellSize=2;
    bool isFirst=true;
    mutex bufferMutex;
    bool gridMode=true;
    bool fillMode=true;
    map<tuple<int, int>, string> lastFrame;
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
    vector<vector<string>> gPhoto();

};

class SilverConsole {

};

    void draw(Vec3 pos, string c);
    void drawLine(Vec3 start, Vec3 end, string c);
    void drawRectangle(Vec3 topLeft, int width, int height, string c);
    void drawCircle(Vec3 center, int radius, string c);
    void drawCircleHollow(Vec3 center, int radius, string c);
    void drawRectangleHollow(Vec3 topLeft, int width, int height, string c);

    void applyScript(const string& objectName, int number, const string& script);
    void executeScript(const string& script, const string& functionName);
    void runScript(mesh& prop);
    void runStart();
    void kill(const string& objectName, const variant<int, vector<int>>& number);
    void revive(const string& objectName, const variant<int, vector<int>>& number);

    void destroy(const string& objectName, const variant<int, vector<int>>& number);

    void gotoxy(int x, int y );
    int put(string objectName, Vec3 position);

    vector <int> seek(string objectName);
    vector <int> seekUnique(string objectName);
    vector<string> spriteAt3(Vec3 pos);
    vector<string> spriteAt2(Vec3 pos);
    void bringToTop(const string& name, int number);

class Keyboard {
public:
    map <char, int> keys;
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

    ThreadInfo() = default; 
    ThreadInfo(const ThreadInfo&) = delete; 
    ThreadInfo& operator=(const ThreadInfo&) = delete; 

};

        map<string, shared_ptr<ThreadInfo>> threads; 

        mutex thread_mutex; 
        vector<thread> activeThreads; 

    public:
        void tRun(function<void()> func) ;

        void dRun(function<void()> func, float time);

        void applyFunction(const string& name, int number, function<void(int)> func, char mode, ...);
        void createThread(const string& name, function<void()> func) ;
        void destroyThread(const string& name) ;

        void stopAllThreads() ;

        void pauseThread(const string& name) ;

        void resumeThread(const string& name) ;
};

class Timer {
public:

    void update(string name) ;

    float getDeltaTime(string name) const ;

    void startTimer(string name);

    void stopTimer(string name) ;

    void resetTimer(string name) ;

    long long getDuration(string name) const ;

private:
    map<string,bool> running;
    map<string,long long> duration; 
    map<string,chrono::high_resolution_clock::time_point> startTime;
    map<string,chrono::high_resolution_clock::time_point> lastTime;
    map<string,float> deltaTime; 
};

    void clear();
    void clean(int x1, int y1, int x2, int y2);
    void loadChunk(const string &file);
    void wait(float time);

    vector<mesh> MeshAt(Vec3 pos, ...) ;
    vector<mesh> GetMeshOf(string name, variant<int, vector<int>> number);

    vector<Vec3> getLocation(const string& name, int number);
    vector<int> getLocationX(const string& name, int number);
    vector<int> getLocationY(const string& name, int number);
    vector<int> getLocationZ(const string& name, int number);
    void PlaceMesh(mesh m, Vec3 pos);
Camera camera;
Keyboard keyboard;
Timer timer;
Threading threading;
Mouse mouse;
};

void startLoading();

extern Silver silver;
extern Vec3 Vector3;
extern World sprites;
extern World killedSprites;

extern map <string, objects> globalObjects;
#endif
