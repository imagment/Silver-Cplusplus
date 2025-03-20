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
World killedSprites;
const World emptyWorld;
std::map<std::string, Actor> Prefabs;

bool debugMode = true;
bool isFirstCameraOutput = true;
Rect StageArea = Rect(-50, -50, 100, 100);

int nextObjectID = 0;

Actor* InstanceIDToActor(int objID) {
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

void Destroy(Actor* actor) {
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
/*
void Rectangle(Actor &object, Vector3 topLeft, int width, int height) {
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      Vector3 pos = {topLeft.x + i, topLeft.y + j, topLeft.z};
      object.addObject();
    }
  }
}
void RectangleHollow(Actor &object, const Vector3 &topLeft, int width,
                     int height) {
  for (int i = 0; i < width; ++i) {
    object.addObjectAt({topLeft.x + i, topLeft.y, topLeft.z});
    object.addObjectAt({topLeft.x + i, topLeft.y + height - 1, topLeft.z});
  }

  for (int j = 1; j < height - 1; ++j) {
    object.addObjectAt({topLeft.x, topLeft.y + j, topLeft.z});
    object.addObjectAt({topLeft.x + width - 1, topLeft.y + j, topLeft.z});
  }
}

void Circle(Actor &object, const Vector3 &center, int radius) {
  for (int y = center.y - radius; y <= center.y + radius; ++y) {
    for (int x = center.x - radius; x <= center.x + radius; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      if (dx * dx + dy * dy <= radius * radius) {
        object.addObjectAt({(double)x, (double)y, center.z});
      }
    }
  }
}

void Line(Actor &object, const Vector3 &start, const Vector3 &end) {
  int x1 = start.x, y1 = start.y;
  int x2 = end.x, y2 = end.y;

  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;

  while (true) {
    object.addObjectAt({(double)x1, (double)y1, start.z});
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

void Oval(Actor &object, const Vector3 &center, const Vector3 &scale) {
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      if ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <=
          (scale.x * scale.x * scale.y * scale.y)) {
        object.addObjectAt({(double)x, (double)y, center.z});
      }
    }
  }
}

void OvalHollow(Actor &object, const Vector3 &center, const Vector3 &scale) {
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
        object.addObjectAt({(double)x, (double)y, center.z});
      }
    }
  }
}

void CircleHollow(Actor &object, const Vector3 &center, int radius) {
  for (int y = center.y - radius; y <= center.y + radius; ++y) {
    for (int x = center.x - radius; x <= center.x + radius; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;
      int distanceSquared = dx * dx + dy * dy;

      if (distanceSquared >= (radius - 1) * (radius - 1) &&
          distanceSquared <= radius * radius) {
        object.addObjectAt({(double)x, (double)y, center.z});
      }
    }
  }
}

void sprayRectangle(Actor &object, int spawns, const Vector3 &center,
                    const Vector3 &scale) {
  srand(static_cast<unsigned>(time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    int offsetX = rand() % static_cast<int>(scale.x);
    int offsetY = rand() % static_cast<int>(scale.y);

    Vector3 position = {center.x + offsetX, center.y + offsetY, center.z};
    object.addObjectAt(position);
  }
}

void sprayOval(Actor &object, int spawns, const Vector3 &center,
               const Vector3 &scale) {
  srand(static_cast<unsigned>(time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    double angle = static_cast<double>(rand()) / RAND_MAX * 2 * M_PI;
    double distanceX = (static_cast<double>(rand()) / RAND_MAX) * scale.x;
    double distanceY = (static_cast<double>(rand()) / RAND_MAX) * scale.y;

    Vector3 position = {center.x + static_cast<int>(distanceX * cos(angle)),
                        center.y + static_cast<int>(distanceY * sin(angle)),
                        center.z};
    object.addObjectAt(position);
  }
}

void spray(Actor &object, int spawns, const Vector3 &center, int range) {
  srand(static_cast<unsigned>(time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    int offsetX = rand() % (2 * range + 1) - range;
    int offsetY = rand() % (2 * range + 1) - range;

    Vector3 position = {center.x + offsetX, center.y + offsetY, center.z};
    object.addObjectAt(position);
  }
}

void sprayLine(Actor &object, int spawns, const Vector3 &start,
               const Vector3 &end) {
  srand(static_cast<unsigned>(time(nullptr)));

  int dx = end.x - start.x;
  int dy = end.y - start.y;
  int dz = end.z - start.z;

  for (int i = 0; i < spawns; ++i) {
    double t = static_cast<double>(rand()) / RAND_MAX;

    Vector3 position =
        (static_cast<int>(start.x + t * dx), static_cast<int>(start.y + t * dy),
         static_cast<int>(start.z + t * dz));
    object.addObjectAt(position);
  }
}
*/
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
  Actor* target = this;

  objectID = nextObjectID;
  Workspace[nextObjectID] = target;
  nextObjectID++;
  return;
}

void Actor::RemoveObject() {
  Actor* target = this;
  
  // Find the object ID to remove
  int id = objectID;

  // Remove the object from the Workspace
  auto it = std::find_if(Workspace.begin(), Workspace.end(),
    [&target](const std::pair<const int, Actor*>& p) {
        return p.second == target;  // Compare the Actor* part of the pair
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

/*
void setObjectXY(const variant<int, vector<int>> objectID, Vector2 pos) {
  int x = pos.x;
  int y = pos.y;

  vector<int> idsToUpdate;

  if (holds_alternative<int>(objectID)) {
    int singleID = get<int>(objectID);
    if (singleID == all_numbers) {
      for (auto &entry : Workspace) {
        idsToUpdate.push_back(entry.first);
      }
    } else {
      idsToUpdate.push_back(singleID);
    }
  } else {
    const vector<int> &ids = get<vector<int>>(objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id : idsToUpdate) {
    auto it = Workspace.find(id);
    if (it != Workspace.end()) {
      Actor &X = it->second;
      int z = X.GetComponent<Transform>()->position.z;

      X.GetComponent<Transform>()->position = (x, y, z);
    }
  }
}
void setObjectY(const variant<int, vector<int>> objectID, Vector3 pos) {
  int y = pos.y;

  vector<int> idsToUpdate;

  if (holds_alternative<int>(objectID)) {
    int singleID = get<int>(objectID);
    if (singleID == all_numbers) {
      for (auto &entry : Workspace) {
        idsToUpdate.push_back(entry.first);
      }
    } else {
      idsToUpdate.push_back(singleID);
    }
  } else {
    const vector<int> &ids = get<vector<int>>(objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id : idsToUpdate) {
    auto it = Workspace.find(id);
    if (it != Workspace.end()) {
      Actor &X = it->second;
      int z = X.GetComponent<Transform>()->position.z;
      int x = X.GetComponent<Transform>()->position.x;
      X.GetComponent<Transform>()->position = (x, y, z);
    }
  }
}
void setObjectX(const variant<int, vector<int>> objectID, Vector3 pos) {
  int x = pos.x;

  vector<int> idsToUpdate;

  if (holds_alternative<int>(objectID)) {
    int singleID = get<int>(objectID);
    if (singleID == all_numbers) {
      for (auto &entry : Workspace) {
        idsToUpdate.push_back(entry.first);
      }
    } else {
      idsToUpdate.push_back(singleID);
    }
  } else {
    const vector<int> &ids = get<vector<int>>(objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id : idsToUpdate) {
    auto it = Workspace.find(id);
    if (it != Workspace.end()) {
      Actor &X = it->second;
      int y = X.GetComponent<Transform>()->position.y;
      int z = X.GetComponent<Transform>()->position.z;

      X.GetComponent<Transform>()->position = (x, y, z);
    }
  }
}

void setObjectRandom(const variant<int, vector<int>> objectID,
                     const pair<int, int> &xRange,
                     const pair<int, int> &yRange) {
  vector<int> idsToUpdate;

  if (holds_alternative<int>(objectID)) {
    int singleID = get<int>(objectID);
    if (singleID == all_numbers) {
      for (auto &entry : Workspace) {
        idsToUpdate.push_back(entry.first);
      }
    } else {
      idsToUpdate.push_back(singleID);
    }
  } else {
    const vector<int> &ids = get<vector<int>>(objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id : idsToUpdate) {
    auto it = Workspace.find(id);
    if (it != Workspace.end()) {
      Actor &X = it->second;
      int z = X.GetComponent<Transform>()->position.z;

      int x = (xRange.first == xRange.second)
                  ? xRange.first
                  : getRandom(xRange.first, xRange.second);
      int y = (yRange.first == yRange.second)
                  ? yRange.first
                  : getRandom(yRange.first, yRange.second);

      X.GetComponent<Transform>()->position = (x, y, z);
    }
  }
}

void setObjectPositionToTarget(const variant<int, vector<int>> objectIDs,
                               int spriteID) {

  Vector3 targetPos =
      getMeshValue(spriteID).GetComponent<Transform>()->position;

  vector<int> idsToUpdate;
  if (holds_alternative<int>(objectIDs)) {
    int singleID = get<int>(objectIDs);

    idsToUpdate.push_back(singleID);

  } else {
    const vector<int> &idList = get<vector<int>>(objectIDs);
    for (int id : idList) {

      idsToUpdate.push_back(id);
    }
  }

  for (int id : idsToUpdate) {
    auto it = Workspace.find(id);
    if (it != Workspace.end()) {
      Actor &objMesh = it->second;
      objMesh.GetComponent<Transform>()->position = targetPos;
    }
  }
}
void glideObjectPositionToTarget(const variant<int, vector<int>> objectIDs,
                                 int spriteID, float speed) {

  Vector3 targetPos =
      getMeshValue(spriteID).GetComponent<Transform>()->position;

  int maxSteps = 0;
  vector<tuple<int, int, int>> idsToUpdate;

  if (holds_alternative<int>(objectIDs)) {
    int singleID = get<int>(objectIDs);

    Vector3 currentPos =
        getMeshValue(singleID).GetComponent<Transform>()->position;
    int x_steps = abs(targetPos.x - currentPos.x);
    int y_steps = abs(targetPos.y - currentPos.y);
    maxSteps = max({maxSteps, x_steps, y_steps});

    idsToUpdate.emplace_back(singleID, x_steps, y_steps);
  } else {
    const vector<int> &idList = get<vector<int>>(objectIDs);

    for (int id : idList) {
      Vector3 currentPos = getMeshValue(id).GetComponent<Transform>()->position;
      int x_steps = abs(targetPos.x - currentPos.x);
      int y_steps = abs(targetPos.y - currentPos.y);
      maxSteps = max({maxSteps, x_steps, y_steps});

      idsToUpdate.emplace_back(id, x_steps, y_steps);
    }
  }

  if (maxSteps == 0) {
    return;
  }

  float stepDuration = speed / maxSteps;

  for (int i = 0; i < maxSteps; ++i) {
    wait(stepDuration);

    for (const auto &[id, x_steps, y_steps] : idsToUpdate) {
      Vector3 currentPos = getMeshValue(id).GetComponent<Transform>()->position;

      if (i < x_steps && (i % (maxSteps / max(1, x_steps)) == 0)) {
        int dirX = (targetPos.x > currentPos.x) ? 1 : -1;
        moveObjectX(id, dirX);
      }

      if (i < y_steps && (i % (maxSteps / max(1, y_steps)) == 0)) {
        int dirY = (targetPos.y > currentPos.y) ? 1 : -1;
        moveObjectY(id, dirY);
      }
    }
  }
}

void glideObjectXY(const variant<int, vector<int>> &ids, Vector2 offset,
                   float speed, ...) {
  va_list args;
  va_start(args, speed);

  bool setPosition = false;

  if (args != nullptr) {

    if (va_arg(args, int) == true) {
      setPosition = true;
    }
  }

  va_end(args);
  int maxSteps = 0;
  vector<pair<int, int>> idsToUpdateX;
  vector<pair<int, int>> idsToUpdateY;

  if (holds_alternative<int>(ids)) {
    int singleId = get<int>(ids);

    Vector3 current_pos =
        getMeshValue(singleId).GetComponent<Transform>()->position;

    int targetX = setPosition ? current_pos.x + offset.x : offset.x;
    int targetY = setPosition ? current_pos.y + offset.y : offset.y;

    int stepsX = abs(targetX - current_pos.x);
    int stepsY = abs(targetY - current_pos.y);
    maxSteps = max({maxSteps, stepsX, stepsY});

    idsToUpdateX.emplace_back(singleId, stepsX);
    idsToUpdateY.emplace_back(singleId, stepsY);
  } else {
    const vector<int> &idList = get<vector<int>>(ids);
    for (const auto &id : idList) {

      Vector3 current_pos =
          getMeshValue(id).GetComponent<Transform>()->position;

      int targetX = setPosition ? current_pos.x + offset.x : offset.x;
      int targetY = setPosition ? current_pos.y + offset.y : offset.y;

      int stepsX = abs(targetX - current_pos.x);
      int stepsY = abs(targetY - current_pos.y);
      maxSteps = max({maxSteps, stepsX, stepsY});

      idsToUpdateX.emplace_back(id, stepsX);
      idsToUpdateY.emplace_back(id, stepsY);
    }
  }

  float stepDuration = speed / maxSteps;

  for (int i = 0; i < maxSteps; ++i) {
    wait(stepDuration);

    for (const auto &[id, stepsX] : idsToUpdateX) {
      if (i < stepsX && (i % (maxSteps / stepsX) == 0)) {
        int dirX = (offset.x > 0) ? 1 : -1;
        moveObjectX(id, dirX);
      }
    }

    for (const auto &[id, stepsY] : idsToUpdateY) {
      if (i < stepsY && (i % (maxSteps / stepsY) == 0)) {
        int dirY = (offset.y > 0) ? 1 : -1;
        moveObjectY(id, dirY);
      }
    }
  }
}
*/

void glideObjectXY(const variant<int, vector<int>> &ids, Vector2 offset,
                   float speed, ...) {}
int GetRandom(int min, int max) {
  static mt19937 rng(random_device{}() ^
                     chrono::system_clock::now().time_since_epoch().count());
  uniform_int_distribution<int> dist(min, max);
  return dist(rng);
}

/*
void setObjectPosition(const variant<int, vector<int>> objectID, Vector3 pos) {
  int x = pos.x;
  int y = pos.y;
  int z = pos.z;

  vector<int> idsToUpdate;

  if (holds_alternative<int>(objectID)) {
    int singleID = get<int>(objectID);

    idsToUpdate.push_back(singleID);
  } else {
    const vector<int> &ids = get<vector<int>>(objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id : idsToUpdate) {
    auto it = Workspace.find(id);
    if (it != Workspace.end()) {
      Actor &X = it->second;
      X.GetComponent<Transform>()->position = (x, y, z);
    }
  }
}

void moveObjectXY(const variant<int, vector<int>> objectID, Vector2 pos) {
  int dx = pos.x;
  int dy = pos.y;

  vector<int> idsToUpdate;

  if (holds_alternative<int>(objectID)) {
    int singleID = get<int>(objectID);

    idsToUpdate.push_back(singleID);
  } else {
    const vector<int> &ids = get<vector<int>>(objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id : idsToUpdate) {
    auto it = Workspace.find(id);
    if (it != Workspace.end()) {
      Actor &X = it->second;
      X.GetComponent<Transform>()->position += Vector3(dx, dy, 0);
      int x = X.GetComponent<Transform>()->position.x;
      int y = X.GetComponent<Transform>()->position.y;
      int z = X.GetComponent<Transform>()->position.z;
    }
  }
}

void moveObjectPosition(const variant<int, vector<int>> objectID, Vector2 pos) {
  int dx = pos.x;
  int dy = pos.y;

  vector<int> idsToUpdate;

  if (holds_alternative<int>(objectID)) {
    int singleID = get<int>(objectID);

    idsToUpdate.push_back(singleID);
  } else {
    const vector<int> &ids = get<vector<int>>(objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id : idsToUpdate) {
    auto it = Workspace.find(id);
    if (it != Workspace.end()) {
      Actor &X = it->second;
      X.GetComponent<Transform>()->position += Vector2(dx, dy);
      int x = X.GetComponent<Transform>()->position.x;
      int y = X.GetComponent<Transform>()->position.y;
      int z = X.GetComponent<Transform>()->position.z;
    }
  }
}

void moveObjectX(const variant<int, vector<int>> objectID, int x_offset) {
  int dx = x_offset;
  vector<int> idsToUpdate;

  if (holds_alternative<int>(objectID)) {
    int singleID = get<int>(objectID);

    idsToUpdate.push_back(singleID);
  } else {
    const vector<int> &ids = get<vector<int>>(objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id : idsToUpdate) {
    auto it = Workspace.find(id);
    if (it != Workspace.end()) {
      Actor &X = it->second;
      X.GetComponent<Transform>()->position += Vector3(dx, 0, 0);
      int x = X.GetComponent<Transform>()->position.x;
      int y = X.GetComponent<Transform>()->position.y;
      int z = X.GetComponent<Transform>()->position.z;
    }
  }
}

void moveObjectY(const variant<int, vector<int>> objectID, int y_offset) {
  int dy = y_offset;
  vector<int> idsToUpdate;

  if (holds_alternative<int>(objectID)) {
    int singleID = get<int>(objectID);

    idsToUpdate.push_back(singleID);
  } else {
    const vector<int> &ids = get<vector<int>>(objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id : idsToUpdate) {
    auto it = Workspace.find(id);
    if (it != Workspace.end()) {
      Actor &X = it->second;
      X.GetComponent<Transform>()->position += Vector3(0, dy, 0);
      int x = X.GetComponent<Transform>()->position.x;
      int y = X.GetComponent<Transform>()->position.y;
      int z = X.GetComponent<Transform>()->position.z;
    }
  }
}

void glideObject(const variant<int, vector<int>> &ids, int offset,
                 bool isYMovement, long long speed, bool setPosition) {
  vector<pair<int, int>> idsToUpdate;
  int maxSteps = 0;

  // Determine if we're working with a single ID or a list of IDs
  if (holds_alternative<int>(ids)) {
    int singleId = get<int>(ids);
    Vector3 current_pos =
        getMeshValue(singleId).GetComponent<Transform>()->position;
    int targetPos = setPosition ? offset
                                : (isYMovement ? current_pos.y + offset
                                               : current_pos.x + offset);
    int steps = abs(targetPos - (isYMovement ? current_pos.y : current_pos.x));
    maxSteps = std::max(maxSteps, steps);
    idsToUpdate.emplace_back(singleId, steps);
  } else {
    const vector<int> &idList = get<vector<int>>(ids);
    for (const auto &id : idList) {
      Vector3 current_pos =
          getMeshValue(id).GetComponent<Transform>()->position;
      int targetPos = setPosition ? offset
                                  : (isYMovement ? current_pos.y + offset
                                                 : current_pos.x + offset);
      int steps =
          abs(targetPos - (isYMovement ? current_pos.y : current_pos.x));
      maxSteps = std::max(maxSteps, steps);
      idsToUpdate.emplace_back(id, steps);
    }
  }

  // Calculate the step duration based on the FPS and total steps

  // Use a lambda function for each thread to move an object
  auto moveObjectThread = [&](int id, int steps) {
    for (int i = 0; i < steps; ++i) {
      // Use std::chrono for waiting between each step
      int stepDuration = speed / steps;

      std::this_thread::sleep_for(std::chrono::milliseconds(stepDuration));

      if (isYMovement) {
        int dir = (offset > 0) ? 1 : -1;
        moveObjectY(id, dir);
      } else {
        int dir = (offset > 0) ? 1 : -1;
        moveObjectX(id, dir);
      }
    }
  };

  // Create and start a thread for each object
  vector<std::thread> threads;
  for (const auto &[id, steps] : idsToUpdate) {
    threads.push_back(std::thread(moveObjectThread, id, steps));
  }

  // Wait for all threads to finish
  for (auto &t : threads) {
    t.join();
  }
}

// Wrapper for glideObjectY
void glideObjectY(const variant<int, vector<int>> &ids, int y_offset,
                  long long speed, ...) {
  va_list args;
  va_start(args, speed);
  bool setPosition = false;

  if (args != nullptr) {
    if (va_arg(args, int) == true) {
      setPosition = true;
    }
  }
  va_end(args);

  glideObject(ids, y_offset, true, speed, setPosition);
}

// Wrapper for glideObjectX
void glideObjectX(const variant<int, vector<int>> &ids, int x_offset,
                  long long speed, ...) {
  va_list args;
  va_start(args, speed);
  bool setPosition = false;

  if (args != nullptr) {
    if (va_arg(args, int) == true) {
      setPosition = true;
    }
  }
  va_end(args);

  glideObject(ids, x_offset, false, speed, setPosition);
}

void glideObjectRandom(const variant<int, vector<int>> &ids,
                       const pair<int, int> &xRange,
                       const pair<int, int> &yRange, float speed) {
  int maxSteps = 0;
  vector<tuple<int, int, int, Vector3>> idsToUpdate;

  auto getRandom = [](int min, int max) {
    static mt19937 rng(random_device{}());
    uniform_int_distribution<int> dist(min, max);
    return dist(rng);
  };

  if (holds_alternative<int>(ids)) {
    int singleId = get<int>(ids);

    Vector3 current_pos =
        getMeshValue(singleId).GetComponent<Transform>()->position;
    int targetX = getRandom(xRange.first, xRange.second);
    int targetY = getRandom(yRange.first, yRange.second);

    int x_steps = abs(targetX - current_pos.x);
    int y_steps = abs(targetY - current_pos.y);
    maxSteps = max({maxSteps, x_steps, y_steps});

    idsToUpdate.emplace_back(singleId, x_steps, y_steps,
                             Vector3(targetX, targetY, (int)current_pos.z));
  } else {
    const vector<int> &idList = get<vector<int>>(ids);
    for (const auto &id : idList) {

      Vector3 current_pos =
          getMeshValue(id).GetComponent<Transform>()->position;
      int targetX = getRandom(xRange.first, xRange.second);
      int targetY = getRandom(yRange.first, yRange.second);

      int x_steps = abs(targetX - current_pos.x);
      int y_steps = abs(targetY - current_pos.y);
      maxSteps = max({maxSteps, x_steps, y_steps});

      idsToUpdate.emplace_back(id, x_steps, y_steps,
                               Vector3(targetX, targetY, (int)current_pos.z));
    }
  }

  float stepDuration = speed / maxSteps;

  for (int i = 0; i < maxSteps; ++i) {
    wait(stepDuration);

    for (const auto &[id, x_steps, y_steps, target_pos] : idsToUpdate) {
      Vector3 current_pos =
          getMeshValue(id).GetComponent<Transform>()->position;

      if (i < x_steps && (i % (maxSteps / x_steps) == 0)) {
        int dirX = (target_pos.x > current_pos.x) ? 1 : -1;
        moveObjectX(id, dirX);
      }

      if (i < y_steps && (i % (maxSteps / y_steps) == 0)) {
        int dirY = (target_pos.y > current_pos.y) ? 1 : -1;
        moveObjectY(id, dirY);
      }
    }
  }
}
*/

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


vector<Actor*> FindObjectsWithTag(const string tag) {
  vector<Actor*> actors;
  for(auto [key, entry] : Workspace) {
    if(entry->tag == tag) actors.push_back(entry);
  }

  return actors;
}

Actor* FindObjectWithTag(const string tag) {
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
