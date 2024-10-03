#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <string>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <map>
#include <atomic>
#include <sstream>
#include <cstdarg>

#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>

#include <termios.h>
#include <fcntl.h>
#include <variant>
#include <unistd.h>
#include <unordered_set>

#include "game.hpp"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

#define MAX_TOKEN_SIZE 1000
#define all -1
#define origin -2
#define upKey 65
#define downKey 66
#define leftKey 68
#define rightKey 67
#define escapeKey 27
#define drawing -214743648

#define NUM_THREADS 1

extern int CameraX, CameraY, CameraScaleX, CameraScaleY, CameraDepth;
extern int cameraRotation;
extern int isFlippedHorizontal, isFlippedVertical;

using namespace std;


class Vec3 {
public:
    int x, y, z;

    Vec3(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

    // Addition of two Vec3
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    // Subtraction of two Vec3
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    // Multiplication of Vec3 with an integer scalar
    Vec3 operator*(int scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    // Compound assignment for addition
    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this; // Return the updated object
    }

    // Compound assignment for subtraction
    Vec3& operator-=(const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this; // Return the updated object
    }

    // Compound assignment for multiplication with an integer scalar
    Vec3& operator*=(int scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this; // Return the updated object
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

    // Conversion to Vec3
    operator Vec3() const {
        return Vec3(x, y, 0);
    }

    // Addition of two Vec2
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    // Subtraction of two Vec2
    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }

    // Multiplication of Vec2 with an integer scalar
    Vec2 operator*(int scalar) const {
        return Vec2(x * scalar, y * scalar);
    }
};



struct sprite {
    Vec3 position;
    string name;
    int number;
    vector <string> tag;

    // Constructor to initialize members
    sprite(const Vec3& pos = Vec3(0,0,0), const string& n = "", int num = 0) 
        : position(pos), name(n), number(num) {}
};

struct animation {
    string animationName;  // Name of the animation
    string animData;       // Renamed to avoid conflict
    int currentFrame;

    // Constructor to initialize members
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
    animation* anim=nullptr;
    int transparency = 0; 
    components Components;  
};

//Prefabrications
struct objects {
    string name;
    string shape;
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

struct rectangle {
    int x1;
    int y1;
    int x2;
    int y2;
};

struct zone {
    rectangle* rect;
    string name;
};

extern multimap <tuple<int, int, int>, mesh> sprites;
extern map <tuple<string, int>, mesh> spritesName;
extern map <string, objects> globalObjects;
extern vector<scene> scenes;

class Silver {
    
public:

    void setObjectPositionToSprite(const string& name, const variant<int, vector<int>>& number,
                                       const string& target, int targetNumber);
    void placeSpray(string objectName, int number, int spawns, Vec3 center, int range);
    void placeExactSpray(const string objectName, int number, int count, Vec3 startPosition);
    void createObject(const string name, const string& shape);

    void place(string objectName, int number, Vec3 position);

    void moveObjectXY(const string& name, const variant<int, vector<int>>& number, Vec3 pos) ;
    void moveObjectX(const string& name, const variant<int, vector<int>>& number, int x_offset);
    void moveObjectY(const string& name, const variant<int, vector<int>>& number, int y_offset);
    
    void setObjectRandom(const string& name, const variant<int, vector<int>>& number);
    
    void setObjectX(const string& name, const variant<int, vector<int>>& number, Vec3 pos);
    void setObjectY(const string& name, const variant<int, vector<int>>& number, Vec3 pos);
    void setObjectXY(const string& name, const variant<int, vector<int>>& number, Vec3 pos);
    void glideObjectRandom(const string& name, variant<int, vector <int>>& number, Vec3 position, float speed);
    void moveObjectPosition(const string& name, const variant<int, vector<int>>& number, Vec3 pos);
    void glideObjectXY(string name, const variant <vector <int>, int>& number, Vec3 target_pos, float speed);
    
    void glideObjectPositionToSprite(const string& name, const variant<int, vector<int>>& number,
                                         const string& target, int targetNumber, float speed);
    void setObjectPosition(const string& name, const variant<int, vector<int>>& number, Vec3 pos);

 
    void removeScript(const string objectName, const string& scriptToRemove);
    void glideObjectX(string name, const variant<int, vector<int>>& number, int x_offset, float speed);
    void glideObjectY(string name, const variant<int, vector<int>>& number, int y_offset, float speed);
    void applyComponent(const string object, int number, const string component, ...);
    void setWorldBounds(Vec3 world);

    void setRawMode();
    void resetMode();
    pair <int,int> getConsoleSize();
    Vec2 getConsoleCenter();
    void stopVMouse();
    void startVMouse(int l, int r, int u, int d, int c);
    
    
class Camera {
public:
    void setCam3(Vec3 pos, Vec3 scale);
    void setCam2(Vec3 pos, Vec3 scale);
    void printCam();
    void flipCamera(int X, int Y); //
    

    void SetCameraFlip(int X, int Y);
    void pivotCamera(int angle); //
    void addPivotCamera(int angle); //
    void shakeCameraOnce(float intensity); 
    void shakeCamera(float intensity, int shakes, float delayBetweenShakes);
    void zoomCamera(Vec3 V); //
    void addCameraDepth(int X); //
    void cell(int c);
    void configCameraException(string o, string n);
    void setCameraDepth(int X); //
    void moveCamera(Vec3 V); //
    void startVideo(int FPS);
    void endVideo();
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


// Define a class for managing scripts and execution

    void applyScript(const string& objectName, int number, const string& script);
    void executeScript(const string& script, const string& functionName);
    void runScript(mesh& prop); // Assuming 'mesh' is defined elsewhere
    void runStart();
    
    void tRun(function<void()> func);
    void dRun(function<void()> func, float time);
    void gotoxy(int x, int y );
    void put(string objectName, Vec3 position);
    vector<Vec3*> spriteLocation(const string& spriteName, int number);
    vector <int> Seek(string objectName);
    vector<string> spriteAt3(Vec3 pos);
    vector<string> spriteAt2(Vec3 pos);
    void bringToTop(const string& name, int number);

// Define a class for managing keyboard input
class Keyboard {
public:
    char detectedKey();
    void detectKey();
    bool isKey(char Key);
};

// Define a class for managing photo functions

    
    
    
    
   
    void clear();
    void clean(int x1, int y1, int x2, int y2);
    void loadChunk(const string &file);
    void wait(float time);


Camera camera;
Keyboard keyboard;
};

void startLoading();

extern Silver silver;
extern Vec3 Vector3;
#endif // GAME_H
