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

#define SPActor std::shared_ptr<Actor>

#define until(condition)                                                       \
  {                                                                            \
    while (1) {                                                                \
      if (condition)                                                           \
        break;                                                                 \
    }                                                                          \
  }

class Actor; // Forward declaration

using World = std::unordered_map<int, std::shared_ptr<Actor>>;


extern Rect StageArea;
extern std::map<std::string, Actor> Prefabs; // A collection to store pre-made objects
extern World Workspace;

extern bool debugMode;

class Component {
protected:
  Actor* parent; // Keeping it lowercase as intended

public:
  explicit Component(Actor* parent)
      : parent(parent) {} // Ensure parent is always initialized
  // Pure virtual Clone method
    virtual std::shared_ptr<Component> Clone() const = 0;
  explicit Component(){} 
  virtual ~Component() = default;

  virtual void Update(float deltaTime) = 0; // Pure virtual function

  Actor* GetParent() const { return parent; } // Safe accessor
  void UnsafeSetParent(Actor* target) {
    parent = target;
  }
};

class Transform : public Component {
public:
    Transform() = default;
    explicit Transform(Actor* parent) : Component(parent) {}

    Transform(const Transform& other) = default;
    Transform& operator=(const Transform& other) = default;

    std::shared_ptr<Component> Clone() const override {
        return std::make_shared<Transform>(*this);
    }

    void Translate(Vector3 offset);
    void Update(float deltaTime) override {}

    Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
    double rotation = 0.0f;
    Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);
};


class SpriteRenderer : public Component {
public:
 std::shared_ptr<Component> Clone() const override {
        return std::make_shared<SpriteRenderer>(*this); // Deep copy
    }

  SpriteRenderer() {};
  explicit SpriteRenderer(std::string newShape) {
    setShape(newShape);
    cleanShape = newShape;

    useRelativePivot = false;
    pivotFactor = Vector2(0.5f, 0.5f);  // Default pivot factor
  }

  // Constructor with shape and pivot
  SpriteRenderer(std::string newShape, Vector2 newPivot) {
    setShape(newShape);
    cleanShape = newShape;
    pivot = newPivot;
  }

  // Constructor with shape, pivot, and relative pivot flag
  SpriteRenderer(bool useRelative, Vector2 newPivot, std::string newShape) {
    useRelativePivot = useRelative;
    setShape(newShape);
    cleanShape = newShape;
    if(!useRelative) pivot = newPivot;
    else pivotFactor = newPivot;  // Default pivot factor
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
  }
  
  explicit SpriteRenderer(Actor* parent) : Component(parent) {}
  std::string getShape();
  void setShape(std::string target);
  void alignShapeTo(double align);
  bool useRelativePivot = false;
  Vector2 pivot = Vector2(0, 0);
  Vector2 pivotFactor = Vector2(0.5, 0.5);
  
  bool isTransparent = false;
  
  Color spriteColor;
  bool useMarkdown = true;
  
  Vector2 GetSize();
  Vector2 GetPivot();
  
  SpriteRenderer(const SpriteRenderer& other)
        : Component(other),
          shape(other.shape),
          cleanShape(other.cleanShape),
          pivot(other.pivot),
          pivotFactor(other.pivotFactor),
          useRelativePivot(other.useRelativePivot),
          isTransparent(other.isTransparent),
          spriteColor(other.spriteColor),
          useMarkdown(other.useMarkdown),
          spriteWidth(other.spriteWidth),
          spriteHeight(other.spriteHeight),
          ansiExtracted(other.ansiExtracted) {
       
        ss = std::stringstream(other.ss.str());
    }

    SpriteRenderer& operator=(const SpriteRenderer& other) {
        if (this != &other) {
            Component::operator=(other);
            shape = other.shape;
            cleanShape = other.cleanShape;
            pivot = other.pivot;
            pivotFactor = other.pivotFactor;
            useRelativePivot = other.useRelativePivot;
            isTransparent = other.isTransparent;
            spriteColor = other.spriteColor;
            useMarkdown = other.useMarkdown;
            spriteWidth = other.spriteWidth;
            spriteHeight = other.spriteHeight;
            ansiExtracted = other.ansiExtracted;

            ss.str("");
            ss.clear();
            ss << other.ss.str();
        }
        return *this;
    }
  
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


class Actor : public std::enable_shared_from_this<Actor>  {
public:
  std::string name;
  
  Actor(const Actor& other);


 template <typename T, typename... Args>
T* AddComponent(Args&&... args) {
    static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

    if (this == nullptr) {
        std::cerr << "Error: Attempting to add component to a null Actor." << std::endl;
        return nullptr;
    }

    for (const auto& comp : objectComponents) {
        if (std::dynamic_pointer_cast<T>(comp)) {
            std::cerr << "Error: Component of type " << typeid(T).name() << " already exists." << std::endl;
            return nullptr;
        }
    }


    // Now safe to create the component
    std::shared_ptr<T> component = std::make_shared<T>(this, std::forward<Args>(args)...);

    // Perform any additional operations (like setting parent) if needed
    component->UnsafeSetParent(this);

    // Add component to objectComponents
    objectComponents.push_back(component);

    return component.get(); // Return raw pointer to the component
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

  Actor() { 
    AddComponent<Transform>();
  }

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
  

   Actor(std::string ActorName, std::vector<std::shared_ptr<Component>> components) : name(ActorName) {
        // Add default Transform component
        AddComponent<Transform>();

        // Add the provided components to the Actor
        for (const auto& component : components) {
            AddComponent(component);
        }
    }


  // Set parent Actor
  void SetParent(std::shared_ptr<Actor> parentActor) {
     if (parentActor) {
            parent = parentActor;
            parent->AddChild(shared_from_this()); // Pass shared_ptr
        }
  }

  // Add a child Actor
  void AddChild(std::shared_ptr<Actor> childActor) { children.push_back(childActor); }

  // Get the parent Actor
  std::shared_ptr<Actor> GetParent() const { return parent; }

  // Get the child Actores
  const std::vector<std::shared_ptr<Actor> > &GetChildren() const { return children; }

  std::map<std::string, int> intValues;
  std::map<std::string, std::string> stringValues;
  std::string tag;

  // Other member functions

  void AddObject();
  void PlaceObject();
  void PlaceObjectAt(Vector3 location);
  void RemoveObject();
  
  int GetInstanceID() {
    return objectID;
  }
private:
  int objectID;
  std::vector<std::shared_ptr<Component>> objectComponents;    // Components of this Actor
  std::shared_ptr<Actor> parent = nullptr; // Parent Actor
  std::vector<std::shared_ptr<Actor> > children;
};


#include "SilverCamera.hpp"

/*
class Fluid : public Component {
public:
 std::shared_ptr<Component> Clone() const override {
        return std::make_shared<Fluid>(*this); // Deep copy
    }
  Fluid(bool isFluid, double diffusionSpeed, int maximumDistance, bool preventFlowing,
          int fluidDepth, int fluidRoot, int fluidParent, double drySpeed, bool isDead)
        : isFluid(isFluid), diffusionSpeed(diffusionSpeed), maximumDistance(maximumDistance),
          preventFlowing(preventFlowing), fluidDepth(fluidDepth), fluidRoot(fluidRoot),
          fluidParent(fluidParent), drySpeed(drySpeed), isDead(isDead) {}

    Fluid(const Fluid& other) { *this = other; }
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
*/

class UI : public Component {
public:
    UI() = default;  // Default constructor

    UI(const UI& other) { *this = other; }  // Copy constructor

    UI& operator=(const UI& other) {
        if (this != &other) {
            // Copy state from `other` (if there are any members, copy them here)
        }
        return *this;
    }

    std::shared_ptr<Component> Clone() const override {
        return std::make_shared<UI>(*this); // Deep copy
    }
  void Update(float deltaTime) {}
};

std::vector<int> Duplicate(const std::variant<int, std::vector<int>> &IDs);
void SetNonBlockingMode();
std::shared_ptr<Actor> InstanceIDToActor(int objID);


bool IsAlive(int obj);

void SetCursorVisibility(bool value);

void Debug(const char *fmt, ...);

void Rectangle(SPActor object, const Rect &rect, double layer);
void RectangleHollow(SPActor object, const Rect &rect, double layer);
void Circle(SPActor object, const Vector3 &center, int radius);
void CircleHollow(SPActor object, const Vector3 &center, int radius);
void Line(SPActor object, const Vector3 &start, const Vector3 &end);
void Oval(const std::string name, int number, Vector3 center, Vector3 scale);
void OvalHollow(SPActor object, const Vector3 &center, const Vector3 &scale);

void SprayRectangle(SPActor object, int spawns, const Rect &rect, double layer);
void SprayOval(SPActor object, int spawns, const Vector3 &center, const Vector3 &scale);
void Spray(SPActor object, int spawns, const Vector3 &center, int range);
void SprayLine(SPActor object, int spawns, const Vector3 &start, const Vector3 &end);
void SprayCircle(SPActor object, int spawns, const Vector3 &center, float radius);

int GetRandom(int min, int max);

void applyFunction(const std::vector<int> &ids, std::function<void(int)> func,
                   char mode, ...);



          

std::vector<std::shared_ptr<Actor>> FindObjectsWithTag(const std::string tag);
std::shared_ptr<Actor> FindObjectWithTag(const std::string tag);

void SetRawMode(bool value);

Vector2 GetConsoleSize();
Vector2 GetConsoleCenter();

void SetConsoleTitle(const std::string title);
void Destroy(std::shared_ptr<Actor> actor);
bool Gotoxy(int x, int y);
double DeltaTime();
void Clear();
void loadChunk(const std::string file);
void Wait(int time);
Actor GetActorValue(int objID);


void Hold();


#endif
