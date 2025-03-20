#ifndef SILVER_HPP
#define SILVER_HPP

// Project-specific headers
#include "SilverColor.hpp"
#include "SilverFluid.hpp"
#include "SilverKeyboard.hpp"
#include "SilverMusic.hpp"
#include "SilverThreading.hpp"
#include "SilverTimer.hpp"
#include "SilverVMouse.hpp"
#include "smath.hpp"

// Third-party libraries
#include "miniaudio.h"

// Standard library headers
#include <algorithm>
#include <atomic>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <iterator>
#include <locale>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <variant>


#define until(condition)                                                       \
  {                                                                            \
    while (1) {                                                                \
      if (condition)                                                           \
        break;                                                                 \
    }                                                                          \
  }

class Actor; // Forward declaration

using World = std::unordered_map<int, Actor*>;


extern Rect StageArea;
extern std::map<std::string, Actor> Prefabs; // A collection to store pre-made objects
extern World Workspace;
extern World KilledSprites;

extern bool debugMode;

class Component {
protected:
  Actor *parent; // Keeping it lowercase as intended

public:
  explicit Component(Actor *m)
      : parent(m) {} // Ensure parent is always initialized
  explicit Component(){} 
  virtual ~Component() = default;

  virtual void Update(float deltaTime) = 0; // Pure virtual function

  Actor *GetParent() const { return parent; } // Safe accessor
  void UnsafeSetParent(Actor *target) {
    parent = target;
  }
};

class Transform : public Component {
public:
  Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
  double rotation = 0.0f;
  Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

  Transform() = default;
  explicit Transform(Actor *m) : Component(m) {}
  void Translate(Vector3 offset);
  // Provide an implementation for the update method
  void Update(float deltaTime) override {
    // Add behavior for updating the Transform (e.g., moving, rotating, etc.)
  }
};

class SpriteRenderer : public Component {
public:
  SpriteRenderer() = default;
  explicit SpriteRenderer(std::string newShape) {
    setShape(newShape);
    cleanShape = newShape;
    spriteWidth = 0;
    spriteHeight = 0;
    useRelativePivot = true;
    pivotFactor = Vector2(0.5f, 0.5f);  // Default pivot factor
  }

  // Constructor with shape and pivot
  SpriteRenderer(std::string newShape, Vector2 newPivot) {
    setShape(newShape);
    cleanShape = newShape;
    pivot = newPivot;
    
    spriteWidth = 0;
    spriteHeight = 0;

  }

  // Constructor with shape, pivot, and relative pivot flag
  SpriteRenderer(bool useRelative, Vector2 newPivot, std::string newShape) {
    useRelativePivot = useRelative;
    setShape(newShape);
    cleanShape = newShape;
    if(!useRelative) pivot = newPivot;
    else pivotFactor = newPivot;  // Default pivot factor
    spriteWidth = 0;
    spriteHeight = 0;
    
  }

  // Constructor with shape, pivot, transparency, markdown, and color
  SpriteRenderer(bool useRelative, Vector2 newPivot, std::string newShape, bool transparent, bool markdown, Color newColor) {
    useRelativePivot = useRelative;
    shape = newShape;
    setShape(newShape);
    if(!useRelative) pivot = newPivot;
    else pivotFactor = newPivot;  // Default pivot factor
    isTransparent = transparent;
    useMarkdown = markdown;
    spriteColor = newColor;
    spriteWidth = 0;
    spriteHeight = 0;
    
  }
  
  explicit SpriteRenderer(Actor *m) : Component(m) {}
  std::string getShape();
  void setShape(std::string target);
  
  bool useRelativePivot = true;
  Vector2 pivot = Vector2(0, 0);
  Vector2 pivotFactor = Vector2(0.5, 0.5);
  
  bool isTransparent = false;
  
  Color spriteColor;
  bool useMarkdown = true;
  
  Vector2 GetSize();
  Vector2 GetPivot();
  
  std::tuple<int, int, int, int> GetPivotBounds();
  std::string GetCellString(int column, int line);
  std::tuple<int, int, int, int> CalculatePivotExpansion();
  void Update(float deltaTime) override {
    
  }
private:
  Vector2 RotatePoint(int column, int line); //Helper function to rotate around the pivot
  std::string shape = "";
  std::string cleanShape = "";
  int spriteHeight = 0;
  int spriteWidth = 0;
  std::stringstream ss;
  std::vector<std::vector<std::string>> ansiExtracted;
};

std::string StripAnsi(const std::string& input) ;
std::vector<std::vector<std::string>> ExtractAnsi(const std::string& input);

class Actor {
public:
  std::string name;

  template <typename T, typename... Args>
T *AddComponent(Args &&...args) {
    static_assert(std::is_base_of<Component, T>::value,
                  "T must inherit from Component");
    if (this == nullptr) {
        std::cerr << "Error: Attempting to add component to a null Actor."
                  << std::endl;
        return nullptr;
    }

    // Check if a component of type T already exists
    for (const auto &comp : objectComponents) {
        if (std::dynamic_pointer_cast<T>(comp)) {
            std::cerr << "Error: Component of type " << typeid(T).name()
                      << " already exists." << std::endl;
            return nullptr;
        }
    }

    // Create and store the component
    T *component = new T(this, std::forward<Args>(args)...);

    // Call UnsafeSetParent before storing
    component->UnsafeSetParent(this);

    objectComponents.push_back(std::shared_ptr<T>(component));

    return component;
}


   template <typename T>
T *AddComponent(std::shared_ptr<T> component) {
    static_assert(std::is_base_of<Component, T>::value,
                  "T must inherit from Component");
    
    if (this == nullptr) {
        std::cerr << "Error: Attempting to add component to a null Actor."
                  << std::endl;
        return nullptr;
    }

    // Check if component of type T already exists
    for (const auto &comp : objectComponents) {
        if (std::dynamic_pointer_cast<T>(comp)) {
            std::cerr << "Error: Component of type " << typeid(T).name()
                      << " already exists." << std::endl;
            return nullptr;
        }
    }

    // Call UnsafeSetParent before storing
    component->UnsafeSetParent(this);

    objectComponents.push_back(component);
    return component.get();
}

  template <typename T> bool RemoveComponent() {
    static_assert(std::is_base_of<Component, T>::value,
                  "T must inherit from Component");

    // Prevent removal of Transform component
    if constexpr (std::is_same_v<T, Transform>) {
      return false; // Transform cannot be removed
    }

    // Find and remove the component
    auto it =
        std::remove_if(objectComponents.begin(), objectComponents.end(),
                       [](const std::shared_ptr<Component> &component) {
                         return dynamic_cast<T *>(component.get()) != nullptr;
                       });

    if (it != objectComponents.end()) {
      objectComponents.erase(
          it, objectComponents.end()); // Remove all matched components
      return true;                     // Removal successful
    }

    return false; // Removal failed (no component of the specified type found)
  }
  
  // Get a component of a specific type
  template <typename T> T *GetComponent() const {
    for (const auto &component : objectComponents) {
      if (auto castedComponent = std::dynamic_pointer_cast<T>(component)) {
        return castedComponent.get();
      }
    }
    return nullptr;
  }

  Actor() { AddComponent<Transform>(); }

  // Constructor that takes only a Actor name
  Actor(std::string ActorName) : name(ActorName) {
    AddComponent<Transform>(); // Add Transform component
  }

  // Constructor that takes both a Actor name and a shape
  Actor(std::string ActorName, std::string shape) : name(ActorName) {
    AddComponent<Transform>(); // Add Transform component
    SpriteRenderer *spriteRenderer =
        AddComponent<SpriteRenderer>(); // Add SpriteRenderer component
    spriteRenderer->setShape(shape);      // Set the shape from the parameter
  }
  
  // Constructor that takes both a Actor name and a shape
  Actor(std::string ActorName, std::string shape, Vector3 position) : name(ActorName) {
    AddComponent<Transform>(); // Add Transform component
    SpriteRenderer *spriteRenderer =
        AddComponent<SpriteRenderer>(); // Add SpriteRenderer component
    spriteRenderer->setShape(shape);         // Set the shape from the parameter
  }

  // Constructor that takes both a Actor name and a shape
  Actor(std::string ActorName, Vector3 position) : name(ActorName) {
    AddComponent<Transform>(); // Add Transform component
  }
   Actor(std::string ActorName, std::vector<std::shared_ptr<Component>> components) : name(ActorName) {
        // Add default Transform component
        AddComponent<Transform>();

        // Add the provided components to the Actor
        for (const auto& component : components) {
            AddComponent(component);
        }
    }


  // Set parent Actor
  void SetParent(Actor *parentActor) {
    parent = parentActor;
    parent-> AddChild(this);
  }

  // Add a child Actor
  void AddChild(Actor *childActor) { children.push_back(childActor); }

  // Get the parent Actor
  Actor *GetParent() const { return parent; }

  // Get the child Actores
  const std::vector<Actor *> &GetChildren() const { return children; }

  unsigned int number = 0;
  std::map<std::string, int> intValues;
  std::map<std::string, std::string> stringValues;
  std::string tag;

  // Other member functions

  void AddObject();
  void RemoveObject();
  
  int GetInstanceID() {
    return objectID;
  }
private:
  int objectID;
  std::vector<std::shared_ptr<Component>> objectComponents;    // Components of this Actor
  Actor *parent = nullptr; // Parent Actor
  std::vector<Actor *> children;
};

class Animation {
public:
  std::vector<std::string> animation;
  void LoadAnimationFromFile(const std::string filename);
  double fps;
  int transition;
  bool immediateTransition;
};

class AnimationManager : public Component {
public:
  void SwitchAnimation(Animation* anim);
  void StopAnimation();
  void PauseAnimation();
  void ResumeAnimation();
  
  void Update();
private:
  Animation* playing = nullptr;
  Animation* nextUp = nullptr;
  int currentFrame = 0;
};

#include "SilverCamera.hpp"

class Fluid : public Component {
public:
  bool isFluid = false;
  double diffusionSpeed = 1.0;
  int maximumDistance = 5;
  bool preventFlowing = false;
  int fluidDepth = 0;
  int fluidRoot = -1;
  int fluidParent = -1;
  double drySpeed = 100.0;
  bool isDead = false;
  void Update() {
    ThreadedFlow(parent->GetInstanceID());
    globalCullingThread(parent->GetInstanceID());
  }
private:
  void globalCullingThread(int rootID);
  void ThreadedFlow(int rootID);
};

class UI : public Component {
  
  void Update() {}
};

std::vector<int> Duplicate(const std::variant<int, std::vector<int>> &IDs);
void SetNonBlockingMode();
Actor *InstanceIDToActor(int objID);

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
bool IsAlive(int obj);
void sprayLine(int spawns, Vector3 start, Vector3 end, std::string name,
               int number);

void SetCursorVisibility(bool value);

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
void Debug(const char *fmt, ...);
void Line(const std::string name, int number, Vector3 start, Vector3 end);

void Oval(const std::string name, int number, Vector3 center, Vector3 scale);
int GetRandom(int min, int max);
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

void glideObjectX(const std::variant<int, std::vector<int>> &ids, int x_offset,
                  long long speed, ...);
void glideObjectY(const std::variant<int, std::vector<int>> &ids, int y_offset,
                  long long speed, ...);

std::vector<Actor*> FindObjectsWithTag(const std::string tag);
Actor* FindObjectWithTag(const std::string tag);

void SetRawMode(bool value);

Vector2 GetConsoleSize();
Vector2 GetConsoleCenter();
void walk(const std::string name, std::variant<std::vector<int>, int> number,
          int steps, int direction);

void SetConsoleTitle(const std::string title);
void Destroy(Actor* actor);
bool Gotoxy(int x, int y);
double DeltaTime();
void Clear();
void loadChunk(const std::string file);
void Wait(int time);
Actor GetActorValue(int objID);


void Hold();


#endif
