#include <algorithm>
#include <atomic>
#include <cmath>
#include <chrono>
#include <condition_variable>
#include <csignal>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits.h>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/ioctl.h>
#include <termios.h>
#include <thread>
#include <tuple>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

mutex scriptMutex;
unordered_set<string> currentPressedKeys;
unordered_set<string> previousPressedKeys;
atomic<bool> keepListening(true);

mutex keyMutex;
World Workspace;

const World emptyWorld;
std::map<std::string, Actor> Prefabs;

bool debugMode = true;
bool isFirstCameraOutput = true;
Rect StageArea = Rect(-50, -50, 100, 100);

int nextObjectID = 0;

Actor::Actor(const Actor& other)
    : name(other.name), objectID(nextObjectID++), intValues(other.intValues),
      stringValues(other.stringValues), tag(other.tag) {

    for (const auto& component : other.objectComponents) {
        objectComponents.push_back(component->Clone());
    }
   
    for (const auto& child : other.children) {
        auto newChild = std::make_shared<Actor>(*child);
        newChild->SetParent(shared_from_this());
        children.push_back(newChild);
    }
}


std::shared_ptr<Actor> InstanceIDToActor(int objID) {
  if(Workspace.find(objID) != Workspace.end()) return Workspace[objID];
  return nullptr;
}


// Global variable to track cursor visibility on program exit
static bool cursorHidden = false;

void CleanUpCursorVisibility() {
    if (cursorHidden) {
        printf("\033[?25h"); // Show cursor
        fflush(stdout);
    }
}

void SetCursorVisibility(bool value) {
  if (value) {
    printf("\033[?25h"); // Show cursor
    cursorHidden = false;
  } else {
    printf("\033[?25l"); // Hide cursor
    cursorHidden = true;

    // Register cleanup on exit
    static bool registered = false;
    if (!registered) {
      std::signal(SIGINT, [](int) {
        CleanUpCursorVisibility();
        exit(0);
      });
      atexit(CleanUpCursorVisibility);
      registered = true;
    }
  }
  fflush(stdout);
}

void Destroy(std::shared_ptr<Actor> actor) {
  actor->RemoveObject();
}

void SetConsoleTitle(const string title) {
  cout << "\033]0;" << title << "\007";
}

void Clear() { system("clear"); }

bool Gotoxy(int x, int y) {
  int consoleWidth = GetConsoleSize().x, consoleHeight = GetConsoleSize().y;
  if (x < 0 || x >= consoleWidth || y < 0 || y >= consoleHeight) {
    return false; // Do nothing if the coordinates are out of bounds
  }
  cout << "\033[" << y + 1 << ";" << x + 1 << "H" << flush;
  return true;
}

// Function to create a filled rectangle
void Rectangle(SPActor object, const Rect &rect, double layer) {
    for (int i = 0; i < rect.width; ++i) {
        for (int j = 0; j < rect.height; ++j) {
            Vector3 pos = Vector3(rect.x + i, rect.y + j, layer); // Placing at the specified layer
            object->PlaceObjectAt(pos);
        }
    }
}

// Function to create a hollow rectangle
void RectangleHollow(SPActor object, const Rect &rect, double layer) {
    // Place objects along the top and bottom edges
    for (int i = 0; i < rect.width; ++i) {
        object->PlaceObjectAt({rect.x + i, rect.y, layer});  // Placing at the specified layer
        object->PlaceObjectAt({rect.x + i, rect.y + rect.height - 1, layer});
    }

    // Place objects along the left and right edges (excluding corners already placed)
    for (int j = 1; j < rect.height - 1; ++j) {
        object->PlaceObjectAt({rect.x, rect.y + j, layer});  // Placing at the specified layer
        object->PlaceObjectAt({rect.x + rect.width - 1, rect.y + j, layer});
    }
}


void Circle(SPActor object, const Vector3 &center, int radius) {
  for (int y = center.y - radius; y <= center.y + radius; ++y) {
    for (int x = center.x - radius; x <= center.x + radius; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      if (dx * dx + dy * dy <= radius * radius) {
        object->PlaceObjectAt({(double)x, (double)y, center.z});
      }
    }
  }
}

void Line(SPActor object, const Vector3 &start, const Vector3 &end) {
  int x1 = start.x, y1 = start.y;
  int x2 = end.x, y2 = end.y;

  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;

  while (true) {
    object->PlaceObjectAt({(double)x1, (double)y1, start.z});
    if (x1 == x2 && y1 == y2)
      break;

    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y1 += sy;
    }
  }
}

void Oval(SPActor object, const Vector3 &center, const Vector3 &scale) {
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      if ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <=
          (scale.x * scale.x * scale.y * scale.y)) {
        object->PlaceObjectAt({(double)x, (double)y, center.z});
      }
    }
  }
}

void OvalHollow(SPActor object, const Vector3 &center, const Vector3 &scale) {
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      int inside =
          (dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x);
      int outer = scale.x * scale.x * scale.y * scale.y;

      Vector3 innerScale = {scale.x - 1, scale.y - 1, scale.z};
      int inner = (dx * dx) * (innerScale.y * innerScale.y) +
                  (dy * dy) * (innerScale.x * innerScale.x);

      if (inside <= outer && inner > outer) {
        object->PlaceObjectAt({(double)x, (double)y, center.z});
      }
    }
  }
}

void CircleHollow(SPActor object, const Vector3 &center, int radius) {
  for (int y = center.y - radius; y <= center.y + radius; ++y) {
    for (int x = center.x - radius; x <= center.x + radius; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;
      int distanceSquared = dx * dx + dy * dy;

      if (distanceSquared >= (radius - 1) * (radius - 1) &&
          distanceSquared <= radius * radius) {
        object->PlaceObjectAt({(double)x, (double)y, center.z});
      }
    }
  }
}

// Function to create a filled rectangle of objects
void SprayRectangle(SPActor object, int spawns, const Rect &rect, double layer) {
    srand(static_cast<unsigned>(time(nullptr)));

    for (int i = 0; i < spawns; ++i) {
        int offsetX = rand() % (int)rect.width;
        int offsetY = rand() % (int)rect.height;

        Vector3 position = {rect.x + offsetX, rect.y + offsetY, layer};
        object->PlaceObjectAt(position);
    }
}

// Function to create a spray of objects in a circular pattern
void SprayCircle(SPActor object, int spawns, const Vector3 &center, float radius) {
    srand(static_cast<unsigned>(time(nullptr)));

    for (int i = 0; i < spawns; ++i) {
        // Random angle in radians (from 0 to 2π)
        double angle = static_cast<double>(rand()) / RAND_MAX * 2 * M_PI;

        // Random radius from the center (up to the specified radius)
        double distance = static_cast<double>(rand()) / RAND_MAX * radius;

        // Calculate the new x and y positions using polar coordinates
        Vector3 position = {
            center.x + static_cast<int>(distance * cos(angle)),
            center.y + static_cast<int>(distance * sin(angle)),
            center.z
        };

        object->PlaceObjectAt(position);
    }
}

// Function to create an oval-shaped spray of objects
void SprayOval(SPActor object, int spawns, const Vector3 &center, const Vector3 &scale) {
    srand(static_cast<unsigned>(time(nullptr)));

    for (int i = 0; i < spawns; ++i) {
        double angle = static_cast<double>(rand()) / RAND_MAX * 2 * M_PI;
        double distanceX = (static_cast<double>(rand()) / RAND_MAX) * scale.x;
        double distanceY = (static_cast<double>(rand()) / RAND_MAX) * scale.y;

        Vector3 position = {
            center.x + static_cast<int>(distanceX * cos(angle)),
            center.y + static_cast<int>(distanceY * sin(angle)),
            center.z
        };
        object->PlaceObjectAt(position);
    }
}

// Function to create a spray of objects within a specified range
void Spray(SPActor object, int spawns, const Vector3 &center, int range) {
    srand(static_cast<unsigned>(time(nullptr)));

    for (int i = 0; i < spawns; ++i) {
        int offsetX = rand() % (2 * range + 1) - range;
        int offsetY = rand() % (2 * range + 1) - range;

        Vector3 position = {center.x + offsetX, center.y + offsetY, center.z};
        object->PlaceObjectAt(position);
    }
}

// Function to create a spray of objects along a line
void SprayLine(SPActor object, int spawns, const Vector3 &start, const Vector3 &end) {
    srand(static_cast<unsigned>(time(nullptr)));

    int dx = end.x - start.x;
    int dy = end.y - start.y;
    int dz = end.z - start.z;

    for (int i = 0; i < spawns; ++i) {
        double t = static_cast<double>(rand()) / RAND_MAX;

        Vector3 position = {
            (start.x + t * dx),
            (start.y + t * dy),
            (start.z + t * dz)
        };
        object->PlaceObjectAt(position);
    }
}



void Debug(const char *fmt, ...) {
  if (!debugMode) {
    return;
  }

  // Format message using vsnprintf
  char buffer[2048]; // Increased buffer size to prevent truncation
  va_list args;
  va_start(args, fmt);
  int result = vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  // Check for truncation
  if (result >= sizeof(buffer)) {
    std::cerr << "Warning: Message truncated due to buffer size!" << std::endl;
  }

  // Construct and execute the system command
  std::string command = "notify-send 'Debug Alarm' \"" + std::string(buffer) + "\"";


  system(command.c_str());
}


double DeltaTime() {
    static std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    double deltaTime = std::chrono::duration<double>(now - lastFrameTime).count();
    lastFrameTime = now;
    return deltaTime;
}


/*
void saveChunk(Vector3 range1, Vector3 range2, const string fileName,
                       const string mode) {
  ofstream fp;

  if (mode == "a") {
    fp.open(fileName, ios::app);
  } else {
    fp.open(fileName, ios::trunc);
  }

  if (!fp.is_open()) {
    debug("From \"void saveChunk(const string file)\" : Error "
                  "opening file.",
                  "e");
    return;
  }

  Vector3 camPos = Camera::CameraPos;
  Vector3 camScale = Camera::CameraScale;
  fp << "📷 " << camPos.x << " " << camPos.y << " " << camPos.z << " "
     << camScale.x << " " << camScale.y << " " << camScale.z << "\n";

  for (auto &objIt : Workspace) {
    Vector3 pos = objIt.second.GetComponent<Transform>()->position;

    if (pos.x >= min(range1.x, range2.x) && pos.x <= max(range1.x, range2.x) &&
        pos.y >= min(range1.y, range2.y) && pos.y <= max(range1.y, range2.y) &&
        pos.z >= min(range1.z, range2.z) && pos.z <= max(range1.z, range2.z)) {

      if (objIt.second.name == "\0") {
        fp << "🗺️ \"" << objIt.second.shape << "\" " << pos.x << " " << pos.y
           << " " << pos.z << "\n";
      } else {
        fp << "➕ " << objIt.second.name << " " << objIt.second.number << " "
           << pos.x << " " << pos.y << " " << pos.z << "\n";
      }
    }
  }

  fp.close();
}

void loadChunk(const string file) {
  ifstream fp(file);

  if (!fp.is_open()) {
    debug("From \"void loadChunk(const string file)\" Error: "
                  "Error opening file.",
                  "e");
    return;
  }

  string buffer;
  int A, B, C, D, E, F;
  int type = -1;

  while (getline(fp, buffer)) {
    if (buffer.rfind("📷", 0) == 0 && type == -1) {
      if (sscanf(buffer.c_str() + sizeof("📷") - 1, "%d %d %d %d %d %d", &A, &B,
                 &C, &D, &E, &F) == 6) {
        Vector3 position = {A, B, C};
        Vector3 scale = {D, E, F};

        Camera::setCam3(position, scale);
        type = 1;
      }
    } else if (buffer.rfind("🗺️", 0) == 0) {
      char context[5000];
      int coord1, coord2, coord3;
      if (sscanf(buffer.c_str() + sizeof("🗺️") - 1, " \"%[^\"]\" %d %d %d",
                 context, &coord1, &coord2, &coord3) == 4) {
        Drawing::draw(Vector3(coord1, coord2, coord3), context);
      }
    } else if (buffer.rfind("💥", 0) == 0) {
      int x1, y1, x2, y2;
      if (sscanf(buffer.c_str() + sizeof("💥") - 1, " %d %d %d %d", &x1, &y1,
                 &x2, &y2) == 4) {
        clean({x1, y1, 0}, {x2, y2, 0});
      }
    } else if (buffer.rfind("➕", 0) == 0) {
      char name[5000];
      int type, coord1, coord2, coord3;

      if (sscanf(buffer.c_str() + sizeof("➕") - 1, " \"%[^\"]\" %d %d %d %d",
                 name, &type, &coord1, &coord2, &coord3) == 5) {
        placeObject(name, type, Vector3(coord1, coord2, coord3));
      }
    }
  }
}
*/

Vector2 GetConsoleSize() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  int width = w.ws_col;
  int height = w.ws_row;
  return Vector2(width, height);
}

Vector2 GetConsoleCenter() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  int width = w.ws_col;
  int height = w.ws_row;
  return Vector2(width - width / 2, height - height / 2);
}

void Transform::Translate(Vector3 offset) { position += offset; }

void Actor::AddObject() {
    // Add the current object to the Workspace
    objectID = nextObjectID;
    Workspace[nextObjectID] = std::make_shared<Actor>(*this); // Deep copy
    nextObjectID++;

    return;
}

void Actor::PlaceObject() {
    // Create a new actor by copying the current one (deep copy)
    auto actorCopy = std::make_shared<Actor>(*this);

    // Set the object ID for the new actor
    actorCopy->objectID = nextObjectID;

    // Add the new actor to the Workspace
    Workspace[nextObjectID] = actorCopy;

    // Increment the nextObjectID for the next object
    nextObjectID++;

    return;
}

void Actor::PlaceObjectAt(Vector3 location) {
    // Create a new actor by copying the current one
    auto actorCopy = std::make_shared<Actor>(*this);

    // Ensure components are cloned properly
    actorCopy->objectComponents.clear();  // Remove shallow copies

    for (const auto& comp : objectComponents) {
        auto clonedComponent = comp->Clone();
        clonedComponent->UnsafeSetParent(actorCopy.get());
        actorCopy->objectComponents.push_back(clonedComponent);
    }

    // Update the new actor's transform position
    auto transform = actorCopy->GetComponent<Transform>();
    if (transform) {
        transform->position = location;
    }

    // Assign a new unique object ID
    actorCopy->objectID = nextObjectID++;

    // Store the duplicate in the workspace
    Workspace[actorCopy->objectID] = actorCopy;
}


void Actor::RemoveObject() {
  std::shared_ptr<Actor> target = shared_from_this();
  
  // Find the object ID to remove
  int id = objectID;

  // Remove the object from the Workspace
  auto it = std::find_if(Workspace.begin(), Workspace.end(),
    [&target](const std::pair<const int, std::shared_ptr<Actor>>& p) {
        return p.second == target;  // Compare the std::shared_ptr<Actor> part of the pair
    });

  if (it != Workspace.end()) {
    Workspace.erase(it);  // Erase the target object from the Workspace
  }

  return;
}


void Hold() {
  while (1)
    Wait(2147483647);
}

void Wait(int time) {
  this_thread::sleep_for(chrono::milliseconds(static_cast<int>(time)));
}

int GetRandom(int min, int max) {
  static mt19937 rng(random_device{}() ^
                     chrono::system_clock::now().time_since_epoch().count());
  uniform_int_distribution<int> dist(min, max);
  return dist(rng);
}


void setRawMode(bool value) {
  struct termios termiosConfig;

  // Retrieve the current terminal attributes
  tcgetattr(STDIN_FILENO, &termiosConfig);

  if (value) {
    // Modify the terminal attributes for raw mode
    termiosConfig.c_lflag &=
        ~(ICANON | ECHO); // Disable canonical mode and echo
  } else {
    // Restore the default behavior
    termiosConfig.c_lflag |= (ICANON | ECHO); // Enable canonical mode and echo
  }

  // Apply the updated terminal attributes
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosConfig);
}

void setNonBlockingMode(bool value) {
  // Get the current file descriptor flags
  int oldFlags = fcntl(STDIN_FILENO, F_GETFL, 0);

  if (value) {
    // Set the non-blocking flag
    fcntl(STDIN_FILENO, F_SETFL, oldFlags | O_NONBLOCK);
  } else {
    // Remove the non-blocking flag
    fcntl(STDIN_FILENO, F_SETFL, oldFlags & ~O_NONBLOCK);
  }
}

vector<Vector3> getOvalPoints(Vector3 center, Vector3 scale) {
  vector<Vector3> points;
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;
      if ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <=
          (scale.x * scale.x * scale.y * scale.y)) {
        points.push_back((x, y, (int)center.z));
      }
    }
  }
  return points;
}

vector<Vector3> getOvalHollowPoints(Vector3 center, Vector3 scale) {
  vector<Vector3> points;
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      bool outer =
          ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <=
           (scale.x * scale.x * scale.y * scale.y));
      scale.x--;
      scale.y--;
      bool inner =
          ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <=
           (scale.x * scale.x * scale.y * scale.y));
      scale.x++;
      scale.y++;

      if (outer && !inner) {
        points.push_back((x, y, (int)center.z));
      }
    }
  }
  return points;
}

vector<Vector3> getLinePoints(Vector3 start, Vector3 end) {
  vector<Vector3> points;

  int x1 = start.x;
  int y1 = start.y;
  int x2 = end.x;
  int y2 = end.y;

  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;

  while (true) {
    points.push_back((x1, y1, (int)start.z));
    if (x1 == x2 && y1 == y2)
      break;

    int e2 = err * 2;
    if (e2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y1 += sy;
    }
  }

  return points;
}

vector<Vector3> getRectanglePoints(Vector3 topLeft, int width, int height) {
  vector<Vector3> points;
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      points.push_back({topLeft.x + i, topLeft.y + j, topLeft.z});
    }
  }
  return points;
}
vector<Vector3> getRectangleHollowPoints(Vector3 topLeft, int width, int height) {
  vector<Vector3> points;

  for (int i = 0; i < width; ++i) {
    points.push_back({topLeft.x + i, topLeft.y, topLeft.z});
    points.push_back({topLeft.x + i, topLeft.y + height - 1, topLeft.z});
  }

  for (int j = 1; j < height - 1; ++j) {
    points.push_back({topLeft.x, topLeft.y + j, topLeft.z});
    points.push_back({topLeft.x + width - 1, topLeft.y + j, topLeft.z});
  }

  return points;
}

bool IsAlive(int obj) { return InstanceIDToActor(obj) != nullptr; }

void ApplyFunction(const std::vector<int> &ids, std::function<void(int)> func,
                   char mode, ...) {
  std::vector<int> targetObjects;
  int key = 0;
  Vector3 pos = {0, 0, 0};
  va_list args;
  va_start(args, mode);

  if (mode == 'k') {
    key = va_arg(args, int);
  } else if (mode == 'm') {
    key = mouseKey;
  }
  va_end(args);

  for (int obj : ids) {
    // Capture variables for the lambda
    auto task = [obj, func, mode, key, pos]() mutable {
      while (IsAlive(obj)) {
        if (mode == 'k' || mode == 'm' || mode == 'c' || mode == 'C') {
          while (true) {
            if (IsKey(key)) {
              if (mode == 'c' || mode == 'C') {
                Vector3 cursorPosition =
                    (cursorPositionX, cursorPositionY, 0);
                if (cursorPosition == pos) {
                  break;
                }
              } else {
                break;
              }
            }
          }
        }

        if (mode == 'h' || mode == 'H') {
          while (true) {
            Vector3 cursorPosition = Vector3(cursorPositionX, cursorPositionY, 0);
            if (cursorPosition == pos) {
              break;
            }
          }
        }

        func(obj); // Execute the provided function
      }
    };

    // Create and manage the thread using ThreadManager
    SThread functionThread;
    functionThread.Enqueue(task);
    functionThread.StartThread();
    functionThread.DetachThread();
  }
}


vector<std::shared_ptr<Actor>> FindObjectsWithTag(const string tag) {
  vector<std::shared_ptr<Actor>> actors;
  for(auto [key, entry] : Workspace) {
    if(entry->tag == tag) actors.push_back(entry);
  }

  return actors;
}

std::shared_ptr<Actor> FindObjectWithTag(const string tag) {
  for(auto [key, entry] : Workspace) {
    if(entry->tag == tag) return entry;
  }

  return nullptr;
}

void SetNonBlockingMode() {
  const char *devicePath = "/dev/input/event0";
  int fd = open(devicePath, O_RDONLY | O_NONBLOCK); // Open in non-blocking mode

  if (fd < 0) { // Check if the file descriptor is valid
    perror("Failed to open input device");
    exit(EXIT_FAILURE); // Exit if the device cannot be opened
  }
}

vector<int> Duplicate(const variant<int, vector<int>> &IDs) {
  vector<int> duplicatedIDs;

  if (holds_alternative<int>(IDs)) {
    int id = get<int>(IDs);

    nextObjectID++;
    Workspace[nextObjectID - 1] = Workspace[id];
    duplicatedIDs.push_back(nextObjectID - 1);

  } else if (holds_alternative<vector<int>>(IDs)) {

    for (int id : get<vector<int>>(IDs)) {
      nextObjectID++;
      Workspace[nextObjectID - 1] = Workspace[id];
      duplicatedIDs.push_back(nextObjectID - 1);
    }
  }

  return duplicatedIDs;
}
