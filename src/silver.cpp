
#include "silver.hpp"
#include <atomic>
#include <cmath>
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
World workspace;
World killedSprites;
const World emptyWorld;
map<const string, prefab> prefabrications;

namespace Silver {

namespace {
std::map<std::string, std::shared_ptr<ThreadInfo>> threads;

std::mutex thread_mutex;
std::vector<std::thread> activeThreads;
}; // namespace
Vector2 WorldRangeStart = {-25, -25}, WorldRangeEnd = {25, 25};

namespace Camera {

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

Vector2 CameraConsolePos = {0, 0};
int anchor = 0;

Vector3 CameraPos = {0, 0, 0};
Vector3 CameraScale = {0, 0, 0};
int cameraRotation = 0;
bool isFlippedHorizontal = false, isFlippedVertical = false;
int cellSize = 2;
bool isFirst = true;
std::mutex bufferMutex;
bool gridMode = true;
bool fillMode = true;
bool hideMouse = true;
std::map<std::tuple<int, int>, std::string> lastFrame;
}; // namespace Camera

int sprites_count = 0;
namespace Scene {

std::map<std::string, World> savedScenes;

};
}; // namespace Silver

mesh *Silver::getMesh(int objID) { return &workspace[objID]; }
mesh Silver::getMeshValue(int objID) { return workspace[objID]; }

vector<int> Silver::all() {
  vector<int> keys;
  keys.reserve(workspace.size());

  for (const auto &pair : workspace) {
    keys.push_back(pair.first);
  }

  return keys;
}

vector<mesh> Silver::getMeshValuesAt2(Vector2 pos) {
  vector<mesh> result;

  for (auto &entry : workspace) {
    if (entry.second.position.x == pos.x && entry.second.position.y == pos.y)
      result.push_back(entry.second);
  }

  return result;
}

vector<mesh> Silver::getMeshValuesAt3(Vector3 pos) {
  vector<mesh> result;

  for (auto &entry : workspace) {
    if (entry.second.position == pos)
      result.push_back(entry.second);
  }

  return result;
}

void Silver::setWorldBounds(Vector2 start, Vector2 end) {
  WorldRangeStart = start;
  WorldRangeEnd = end;
}

vector<int> Silver::findObjects(const string name,
                                variant<vector<int>, int> number) {
  vector<int> numbers;

  for (auto &entry : workspace) {

    if (entry.second.name == name) {

      if (holds_alternative<vector<int>>(number)) {
        vector<int> numList = get<vector<int>>(number);

        if (find(numList.begin(), numList.end(), entry.second.number) !=
            numList.end()) {
          numbers.push_back(entry.first);
        }
      } else if (holds_alternative<int>(number)) {
        int num = get<int>(number);

        if (entry.second.number == num || num == all_numbers) {
          numbers.push_back(entry.first);
        }
      }
    }
  }

  return numbers;
}

void Silver::destroy(variant<int, vector<int>> objIDs) {
  vector<int> keysToRemove;

  if (holds_alternative<int>(objIDs)) {
    int objID = get<int>(objIDs);
    if (workspace.find(objID) != workspace.end()) {
      keysToRemove.push_back(objID);
    }
  } else {
    for (int objID : get<vector<int>>(objIDs)) {
      if (workspace.find(objID) != workspace.end()) {
        keysToRemove.push_back(objID);
      }
    }
  }

  for (const int &key : keysToRemove) {
    workspace.erase(key);
  }
}

void Silver::kill(variant<int, vector<int>> objIDs) {
  vector<int> keysToKill;

  if (holds_alternative<int>(objIDs)) {
    int objID = get<int>(objIDs);
    if (workspace.find(objID) != workspace.end()) {
      keysToKill.push_back(objID);
    }
  } else {
    for (int objID : get<vector<int>>(objIDs)) {
      if (workspace.find(objID) != workspace.end()) {
        keysToKill.push_back(objID);
      }
    }
  }

  for (const int &key : keysToKill) {
    killedSprites[key] = workspace[key];
    workspace.erase(key);
  }
}

void Silver::revive(variant<int, vector<int>> objIDs) {
  vector<int> keysToRevive;

  if (holds_alternative<int>(objIDs)) {
    int objID = get<int>(objIDs);
    if (killedSprites.find(objID) != killedSprites.end()) {
      keysToRevive.push_back(objID);
    }
  } else {
    for (int objID : get<vector<int>>(objIDs)) {
      if (killedSprites.find(objID) != killedSprites.end()) {
        keysToRevive.push_back(objID);
      }
    }
  }

  for (const int &key : keysToRevive) {
    workspace[key] = killedSprites[key];
    killedSprites.erase(key);
  }
}

void Silver::Scene::saveCurrentWorld(const string name) {
  savedScenes[name] = workspace;
}

void Silver::Scene::loadScene(const string name) {
  auto it = savedScenes.find(name);
  if (it != savedScenes.end()) {
    workspace = it->second;
  }
}

void Silver::Scene::resetScene() {
  World empty;
  workspace = empty;
}

Vector2 Silver::Camera::getScreenPosition(Vector3 pos) {
  int worldX = pos.x;
  int worldY = pos.y;
  float radians = Silver::Camera::cameraRotation * (M_PI / 180.0);
  float cosTheta = cos(radians);
  float sinTheta = sin(radians);

  int centerX = Silver::Camera::CameraScale.x / 2;
  int centerY = Silver::Camera::CameraScale.y / 2;

  float dx = worldX - Silver::Camera::CameraPos.x;
  float dy = worldY - Silver::Camera::CameraPos.y;

  int rotatedX = round(cosTheta * dx + sinTheta * dy);
  int rotatedY = round(-sinTheta * dx + cosTheta * dy);

  if (Silver::Camera::isFlippedHorizontal == -1) {
    rotatedX = Silver::Camera::CameraScale.x - rotatedX - 1;
  }
  if (Silver::Camera::isFlippedVertical == -1) {
    rotatedY = Silver::Camera::CameraScale.y - rotatedY - 1;
  }

  int screenX = rotatedX + centerX;
  int screenY = rotatedY + centerY;

  return {screenX, screenY};
}

void Silver::Camera::flipCamera(bool x, bool y) {
  if (x) {
    Silver::Camera::isFlippedHorizontal =
        !Silver::Camera::isFlippedHorizontal; // Toggle horizontal flip
  }
  if (y) {
    Silver::Camera::isFlippedVertical =
        !Silver::Camera::isFlippedVertical; // Toggle vertical flip
  }
}

void Silver::Camera::setCameraFlip(bool x, bool y) {
  Silver::Camera::isFlippedHorizontal =
      x; // Set horizontal flip (true = flipped, false = not flipped)
  Silver::Camera::isFlippedVertical =
      y; // Set vertical flip (true = flipped, false = not flipped)
}

void Silver::Camera::pivotCamera(int angle) {
  Silver::Camera::cameraRotation = angle;
}

void Silver::Camera::addPivotCamera(int angle) {
  Silver::Camera::cameraRotation += angle;
}

void Silver::Camera::cell(int c) { Silver::Camera::cellSize = c; }

void Silver::Camera::configCameraException(const string n) { null_rep = n; }

void Silver::Camera::setCam3(Vector3 pos, Vector3 scale) {
  Silver::Camera::CameraPos = pos;
  Silver::Camera::CameraScale = scale;
}

void Silver::Camera::setCam2(Vector2 pos, Vector2 scale) {
  Silver::Camera::CameraPos = {pos.x, pos.y, Silver::Camera::CameraPos.z};
  Silver::Camera::CameraScale = {pos.x, pos.y, Silver::Camera::CameraScale.z};
}

int getObjectIdAtCoordinates(Vector3 pos) {

  for (const auto &entry : workspace) {
    const auto &object = entry.second;

    if (pos == entry.second.position) {
      return entry.first;
    }
  }

  return -1;
}

string truncateToCellSize(const string cellContent, int cellSize) {
  string result;
  int currentWidth = 0;
  size_t i = 0;

  while (i < cellContent.size()) {

    unsigned char ch = cellContent[i];
    int charBytes = 1;

    if ((ch & 0x80) != 0) {
      if ((ch & 0xE0) == 0xC0) {
        charBytes = 2;
      } else if ((ch & 0xF0) == 0xE0) {
        charBytes = 3;
      } else if ((ch & 0xF8) == 0xF0) {
        charBytes = 4;
      }
    }

    if (currentWidth + 1 > cellSize) {
      break;
    }

    result += cellContent.substr(i, charBytes);
    currentWidth += 1;

    i += charBytes;
  }

  return result;
}

void displayText(const string text, int scaleX, int maxLeftWidth, bool isTop,
                 int offsetX) {
  if (!text.empty()) {
    stringstream ss(text);
    string line;
    cout << "\n";
    while (getline(ss, line)) {
      int padding =
          (scaleX * Silver::Camera::cellSize - line.size()) / 2 + offsetX;
      padding = max(0, padding + maxLeftWidth);
      cout << string(padding, ' ') << line << endl;
    }
  }
}

string join(const vector<string> &lines) {
  string result;
  for (const auto &line : lines) {
    result += line + "\n";
  }
  if (!result.empty()) {
    result.pop_back();
  }
  return result;
}

void Silver::Camera::printCam() {
  if (isFirst) {
    Silver::setRawMode(true);
    Silver::clear();
  }

  vector<vector<string>> buffer(
      Silver::Camera::CameraScale.y,
      vector<string>(Silver::Camera::CameraScale.x, null_rep));

  int worldRangeMinX =
      std::min(Silver::WorldRangeStart.x, Silver::WorldRangeEnd.x);
  int worldRangeMaxX =
      std::max(Silver::WorldRangeStart.x, Silver::WorldRangeEnd.x);
  int worldRangeMinY =
      std::min(Silver::WorldRangeStart.y, Silver::WorldRangeEnd.y);
  int worldRangeMaxY =
      std::max(Silver::WorldRangeStart.y, Silver::WorldRangeEnd.y);

  float radians = Silver::Camera::cameraRotation * (M_PI / 180.0);
  float cosTheta = cos(radians);
  float sinTheta = sin(radians);

  int centerX = Silver::Camera::CameraScale.x / 2;
  int centerY = Silver::Camera::CameraScale.y / 2;

  if (markOutOfBounds) {
    for (int y = 0; y < Silver::Camera::CameraScale.y; ++y) {
      for (int x = 0; x < Silver::Camera::CameraScale.x; ++x) {
        // Flip screen coordinates if necessary
        int flippedX = (Silver::Camera::isFlippedHorizontal)
                           ? (Silver::Camera::CameraScale.x - x - 1)
                           : x;
        int flippedY = (Silver::Camera::isFlippedVertical)
                           ? (Silver::Camera::CameraScale.y - y - 1)
                           : y;

        // Convert flipped screen coordinates to world coordinates
        int worldX = round((flippedX - centerX) * cosTheta -
                           (flippedY - centerY) * sinTheta) +
                     Silver::Camera::CameraPos.x;
        int worldY = round((flippedX - centerX) * sinTheta +
                           (flippedY - centerY) * cosTheta) +
                     Silver::Camera::CameraPos.y;

        // Calculate actual world range boundaries

        // Mark out-of-bound areas
        if (worldX < worldRangeMinX || worldX > worldRangeMaxX ||
            worldY < worldRangeMinY || worldY > worldRangeMaxY) {
          buffer[y][x] =
              out_rep; // Original (non-flipped) screen position is updated
        }
      }
    }
  }

  bool sign = false;
  string signMessage, signIcon, objectName;

  string rightText, leftText, topText, bottomText;
  if (!Silver::Camera::rightText.empty()) {
    rightText = join(Silver::Camera::rightText);
  }

  if (!Silver::Camera::leftText.empty()) {
    leftText = join(Silver::Camera::leftText);
  }

  if (!Silver::Camera::bottomText.empty()) {
    bottomText = join(Silver::Camera::bottomText);
  }

  if (!Silver::Camera::topText.empty()) {
    topText = join(Silver::Camera::topText);
  }

  for (const auto &entry : workspace) {
    const auto &obj = entry.second;

    // Check if the object's position is within the world range for x and y

    int dx, dy, dz;

    if (obj.comp.isUI) {
      dx = (obj.position.x - 1048576) - Silver::Camera::CameraScale.x / 2;
      dy = (obj.position.y - 1048576) - Silver::Camera::CameraScale.y / 2;
    } else {
      dx = obj.position.x - Silver::Camera::CameraPos.x;
      dy = obj.position.y - Silver::Camera::CameraPos.y;
    }

    if (dx < worldRangeMinX || dx > worldRangeMaxX || dy < worldRangeMinY ||
        dy > worldRangeMaxY && cutOutOfBounds) {
      continue; // Skip objects outside the world range
    }

    dz = obj.position.z - Silver::Camera::CameraPos.z;

    int rotatedX = round(cosTheta * dx + sinTheta * dy) + centerX;
    int rotatedY = round(-sinTheta * dx + cosTheta * dy) + centerY;

    if (Silver::Camera::isFlippedHorizontal == -1) {
      rotatedX = Silver::Camera::CameraScale.x - rotatedX - 1;
    }
    if (Silver::Camera::isFlippedVertical == -1) {
      rotatedY = Silver::Camera::CameraScale.y - rotatedY - 1;
    }

    if (rotatedX >= 0 && rotatedX < Silver::Camera::CameraScale.x &&
        rotatedY >= 0 && rotatedY < Silver::Camera::CameraScale.y &&
        dz >= -Silver::Camera::CameraScale.z / 2 &&
        dz <= Silver::Camera::CameraScale.z / 2) {
      bool isTransparent = obj.isTransparent == true;

      if (!printSpaces) {
        isTransparent =
            isTransparent || (obj.shape.find(' ') != std::string::npos ||
                              obj.shape.find('\t') != std::string::npos);
      }
      if (!isTransparent) {
        buffer[rotatedY][rotatedX] = obj.shape;

        if (!obj.comp.signMessage.empty()) {
          signMessage = obj.comp.signMessage;
          sign = true;
          signIcon = obj.shape;
          objectName = obj.name;
        }
      }
    }
  }

  int mouseX = Silver::Mouse::cursorPositionX;
  int mouseY = Silver::Mouse::cursorPositionY;

  vector<string> leftTextLines, rightTextLines;
  int leftTextLinesCount = 0, rightTextLinesCount = 0;

  if (!Silver::Camera::rightText.empty()) {
    std::string rightTextConcat = "";
    for (size_t i = 0; i < Silver::Camera::rightText.size(); ++i) {
      rightTextConcat += Silver::Camera::rightText[i];
      if (i != Silver::Camera::rightText.size() - 1) {
        rightTextConcat += '\n';
      }
    }

    stringstream ss(rightTextConcat);
    string line;
    while (getline(ss, line)) {
      rightTextLines.push_back(line);
      rightTextLinesCount++;
    }
  }

  int maxLeftWidth = 0, maxRightWidth = 0;
  for (const auto &line : leftTextLines) {
    maxLeftWidth = std::max(maxLeftWidth, static_cast<int>(line.size()));
  }
  for (const auto &line : rightTextLines) {
    maxRightWidth = std::max(maxRightWidth, static_cast<int>(line.size()));
  }

  int tl = Silver::Camera::leftAlign == 0
               ? Silver::Camera::CameraScale.y / 2 - leftTextLinesCount / 2
               : (Silver::Camera::leftAlign == 1
                      ? 0
                      : Silver::Camera::CameraScale.y - leftTextLinesCount);
  int tr = Silver::Camera::rightAlign == 0
               ? Silver::Camera::CameraScale.y / 2 - rightTextLinesCount / 2
               : (Silver::Camera::rightAlign == 1
                      ? 0
                      : Silver::Camera::CameraScale.y - rightTextLinesCount);

  int offsetX = Silver::Camera::CameraConsolePos.x;
  int offsetY = Silver::Camera::CameraConsolePos.y;

  switch (anchor) {
  case 0:
    break;
  case 1:
    offsetX -= Silver::Camera::CameraScale.x / 2;
    break;
  case 2:
    offsetX -= Silver::Camera::CameraScale.x;
    break;
  case 3:
    offsetY -= Silver::Camera::CameraScale.y / 2;
    break;
  case 4:
    offsetX -= Silver::Camera::CameraScale.x / 2;
    offsetY -= Silver::Camera::CameraScale.y / 2;
    break;
  case 5:
    offsetX -= Silver::Camera::CameraScale.x;
    offsetY -= Silver::Camera::CameraScale.y / 2;
    break;
  case 6:
    offsetY -= Silver::Camera::CameraScale.y;
    break;
  case 7:
    offsetX -= Silver::Camera::CameraScale.x / 2;
    offsetY -= Silver::Camera::CameraScale.y;
    break;
  case 8:
    offsetX -= Silver::Camera::CameraScale.x;
    offsetY -= Silver::Camera::CameraScale.y;
    break;
  }

  offsetX = std::max(0, offsetX);
  offsetY = std::max(0, offsetY);

  for (const auto &line : leftTextLines) {
    maxLeftWidth = std::max(maxLeftWidth, static_cast<int>(line.size()));
  }

  if (!Silver::Camera::leftText.empty()) {
    std::string leftTextConcat = "";
    for (size_t i = 0; i < Silver::Camera::leftText.size(); ++i) {
      leftTextConcat += Silver::Camera::leftText[i];
      if (i != Silver::Camera::leftText.size() - 1) {
        leftTextConcat += '\n';
      }
    }

    stringstream ss(leftTextConcat);
    string line;
    while (getline(ss, line)) {
      leftTextLines.push_back(line);
      leftTextLinesCount++;
    }
  }

  for (const auto &line : leftTextLines) {
    maxLeftWidth = std::max(maxLeftWidth, static_cast<int>(line.size()));
  }

  int topTextOffsetX = offsetX;

  topTextOffsetX += maxLeftWidth;

  int topTextLinesCount = 0;
  int bottomTextLinesCount = 0;
  string topTextConcat = "";
  string bottomTextConcat = "";
  vector<string> topTextLines;
  vector<string> bottomTextLines;

  if (!Silver::Camera::topText.empty()) {
    std::string topTextConcat = "";
    for (size_t i = 0; i < Silver::Camera::topText.size(); ++i) {
      topTextConcat += Silver::Camera::topText[i];
      if (i != Silver::Camera::topText.size() - 1) {
        topTextConcat += '\n';
      }
    }

    std::stringstream ssTop(topTextConcat);
    std::string line;
    while (std::getline(ssTop, line)) {
      topTextLines.push_back(line);
      ++topTextLinesCount;
    }
  }

  if (!Silver::Camera::bottomText.empty()) {
    std::string bottomTextConcat = "";
    for (size_t i = 0; i < Silver::Camera::bottomText.size(); ++i) {
      bottomTextConcat += Silver::Camera::bottomText[i];
      if (i != Silver::Camera::bottomText.size() - 1) {
        bottomTextConcat += '\n';
      }
    }

    std::stringstream ssBottom(bottomTextConcat);
    std::string line;
    while (std::getline(ssBottom, line)) {
      bottomTextLines.push_back(line);
      ++bottomTextLinesCount;
    }
  }

  for (int i = 0; i < topTextLines.size(); ++i) {
    int lineOffsetX = offsetX + maxLeftWidth + 1;
    if (Silver::Camera::topAlign == 1) {
      lineOffsetX += 0;
    } else if (Silver::Camera::topAlign == 2) {
      lineOffsetX +=
          Silver::Camera::CameraScale.x * cellSize - topTextLines[i].size();
    } else if (Silver::Camera::topAlign == 0) {
      lineOffsetX +=
          (Silver::Camera::CameraScale.x * cellSize - topTextLines[i].size()) /
          2;
    }
    int currentY = offsetY + i;
    if (currentY >= 0) {
      Silver::gotoxy(lineOffsetX, currentY);
      cout << topTextLines[i] << flush;
    }
  }

  int renderedHeight = Silver::Camera::CameraScale.y;
  if (!sideLimit) {

    renderedHeight = std::max(
        renderedHeight, std::max(leftTextLinesCount, rightTextLinesCount));
  }

  for (int j = 0; j < renderedHeight; ++j) {
    string leftLine = (j - tl < leftTextLines.size() && j - tl >= 0)
                          ? leftTextLines[j - tl]
                          : "";
    leftLine = string(maxLeftWidth - leftLine.size(), ' ') + leftLine;

    string rightLine = (j - tr < rightTextLines.size() && j - tr >= 0)
                           ? rightTextLines[j - tr]
                           : "";
    rightLine += string(maxRightWidth - rightLine.size(), ' ');

    string line = leftLine;

    if (j < Silver::Camera::CameraScale.y) {

      for (int i = 0; i < Silver::Camera::CameraScale.x; ++i) {
        string cellContent = (!(hideMouse || Silver::Mouse::hideMouse) &&
                              i == mouseX && j == mouseY)
                                 ? Silver::Mouse::mouseIcon
                                 : buffer[j][i];

        if (gridMode && cellContent.size() > Silver::Camera::cellSize) {
          cellContent =
              truncateToCellSize(cellContent, Silver::Camera::cellSize);
        }
        if (fillMode && cellContent.size() < Silver::Camera::cellSize) {
          cellContent.append(Silver::Camera::cellSize - cellContent.size(),
                             ' ');
        }

        line += cellContent;
      }
    } else {

      line +=
          string(Silver::Camera::cellSize * Silver::Camera::CameraScale.x, ' ');
    }

    line += rightLine;

    int currentY = offsetY + j + topTextLinesCount;
    if (currentY >= 0) {

      Silver::gotoxy(offsetX, currentY);
      cout << line << flush;
    }
  }

  for (int i = 0; i < bottomTextLines.size(); ++i) {
    int lineOffsetX = offsetX + maxLeftWidth + 1;
    if (Silver::Camera::bottomAlign == 1) {
      lineOffsetX += 0;
    } else if (Silver::Camera::bottomAlign == 2) {
      lineOffsetX +=
          Silver::Camera::CameraScale.x * cellSize - bottomTextLines[i].size();
    } else if (Silver::Camera::bottomAlign == 0) {
      lineOffsetX += (Silver::Camera::CameraScale.x * cellSize -
                      bottomTextLines[i].size()) /
                     2;
    }
    int currentY =
        offsetY + Silver::Camera::CameraScale.y + topTextLinesCount + i;
    if (currentY >= 0) {
      Silver::gotoxy(lineOffsetX, currentY);
      cout << bottomTextLines[i] << flush;
    }
  }
  isFirst = false;
}

void Silver::setConsoleTitle(const string title) {
  cout << "\033]0;" << title << "\007";
}

void Silver::clear() { system("clear"); }

void Silver::gotoxy(int x, int y) {
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;
  cout << "\033[" << y + 1 << ";" << x + 1 << "H" << flush;
}

void Silver::Drawing::spray(int spawns, Vector3 center, int range, string c) {
  srand(static_cast<unsigned int>(time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    int offsetX = rand() % (2 * range + 1) - range;
    int offsetY = rand() % (2 * range + 1) - range;

    Vector3 position = {center.x + offsetX, center.y + offsetY, center.z};

    draw(position, c);
  }
}

void Silver::Drawing::sprayOval(int spawns, Vector3 center, Vector3 scale,
                                string c) {
  srand(static_cast<unsigned int>(time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    double angle = static_cast<double>(rand()) / RAND_MAX * 2 * M_PI;
    double distanceX = (static_cast<double>(rand()) / RAND_MAX) * scale.x;
    double distanceY = (static_cast<double>(rand()) / RAND_MAX) * scale.y;

    Vector3 position = {center.x + static_cast<int>(distanceX * cos(angle)),
                        center.y + static_cast<int>(distanceY * sin(angle)),
                        center.z};

    draw(position, c);
  }
}
void Silver::Drawing::sprayRectangle(int spawns, Vector3 center, Vector3 scale,
                                     string c) {
  srand(static_cast<unsigned int>(time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    int offsetX = rand() % static_cast<int>(scale.x);
    int offsetY = rand() % static_cast<int>(scale.y);

    Vector3 position = {center.x + offsetX, center.y + offsetY, center.z};

    draw(position, c);
  }
}

void Silver::Drawing::sprayLine(int spawns, Vector3 start, Vector3 end,
                                string c) {
  srand(static_cast<unsigned int>(time(nullptr)));

  int dx = end.x - start.x;
  int dy = end.y - start.y;
  int dz = end.z - start.z;

  for (int i = 0; i < spawns; ++i) {
    double t = static_cast<double>(rand()) / RAND_MAX;

    Vector3 position = {static_cast<int>(start.x + t * dx),
                        static_cast<int>(start.y + t * dy),
                        static_cast<int>(start.z + t * dz)};

    draw(position, c);
  }
}

void Silver::Drawing::Rectangle(Vector3 topLeft, int width, int height,
                                string c) {
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      Vector3 pos = {topLeft.x + i, topLeft.y + j, topLeft.z};
      draw(pos, c);
    }
  }
}
void Silver::Drawing::Line(Vector3 start, Vector3 end, string c) {
  int dx = abs(end.x - start.x), dy = abs(end.y - start.y);
  int sx = (start.x < end.x) ? 1 : -1;
  int sy = (start.y < end.y) ? 1 : -1;
  int err = dx - dy;

  while (true) {
    draw(start, c);
    if (start.x == end.x && start.y == end.y)
      break;
    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      start.x += sx;
    }
    if (e2 < dx) {
      err += dx;
      start.y += sy;
    }
  }
}

void Silver::Drawing::RectangleHollow(Vector3 topLeft, int width, int height,
                                      string c) {

  Line(topLeft, {topLeft.x + width - 1, topLeft.y, topLeft.z}, c);

  Line({topLeft.x, topLeft.y + height - 1, topLeft.z},
       {topLeft.x + width - 1, topLeft.y + height - 1, topLeft.z}, c);

  Line(topLeft, {topLeft.x, topLeft.y + height - 1, topLeft.z}, c);

  Line({topLeft.x + width - 1, topLeft.y, topLeft.z},
       {topLeft.x + width - 1, topLeft.y + height - 1, topLeft.z}, c);
}

void Silver::Drawing::CircleHollow(Vector3 center, int radius, string c) {
  int x = 0, y = radius;
  int d = 3 - 2 * radius;
  while (x <= y) {

    draw({center.x + x, center.y + y, center.z}, c);
    draw({center.x - x, center.y + y, center.z}, c);
    draw({center.x + x, center.y - y, center.z}, c);
    draw({center.x - x, center.y - y, center.z}, c);
    draw({center.x + y, center.y + x, center.z}, c);
    draw({center.x - y, center.y + x, center.z}, c);
    draw({center.x + y, center.y - x, center.z}, c);
    draw({center.x - y, center.y - x, center.z}, c);

    if (d <= 0) {
      d += 4 * x + 6;
    } else {
      d += 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
}

void Silver::Drawing::Oval(Vector3 center, int radiusX, int radiusY, string c) {
  for (int y = -radiusY; y <= radiusY; y++) {
    for (int x = -radiusX; x <= radiusX; x++) {
      if ((x * x * radiusY * radiusY) + (y * y * radiusX * radiusX) <=
          (radiusX * radiusX * radiusY * radiusY)) {
        draw({center.x + x, center.y + y, center.z}, c);
      }
    }
  }
}

void Silver::Drawing::OvalHollow(Vector3 center, int radiusX, int radiusY,
                                 string c) {
  for (int y = -radiusY; y <= radiusY; y++) {
    for (int x = -radiusX; x <= radiusX; x++) {
      int equationValue =
          (x * x * radiusY * radiusY) + (y * y * radiusX * radiusX);
      int threshold = radiusX * radiusX * radiusY * radiusY;
      if (abs(equationValue - threshold) <= radiusX * radiusY) {
        draw({center.x + x, center.y + y, center.z}, c);
      }
    }
  }
}

void Silver::Drawing::draw(Vector3 pos, string c) {
  mesh p;
  p.name = "\0";
  p.position = pos;
  p.number = Silver::sprites_count++;
  p.shape = c;

  workspace.insert({p.number, p});
}

void Silver::Rectangle(const string name, int number, Vector3 topLeft,
                       int width, int height) {
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      Vector3 pos = {topLeft.x + i, topLeft.y + j, topLeft.z};
      Silver::placeObject(name, number, pos);
    }
  }
}

void Silver::RectangleHollow(const string name, int number, Vector3 topLeft,
                             int width, int height) {
  for (int i = 0; i < width; ++i) {
    placeObject(name, number, {topLeft.x + i, topLeft.y, topLeft.z});
    placeObject(name, number,
                {topLeft.x + i, topLeft.y + height - 1, topLeft.z});
  }

  for (int j = 1; j < height - 1; ++j) {
    placeObject(name, number, {topLeft.x, topLeft.y + j, topLeft.z});
    placeObject(name, number,
                {topLeft.x + width - 1, topLeft.y + j, topLeft.z});
  }
}

void Silver::Circle(const string name, int number, Vector3 center, int radius) {
  for (int y = center.y - radius; y <= center.y + radius; ++y) {
    for (int x = center.x - radius; x <= center.x + radius; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      if (dx * dx + dy * dy <= radius * radius) {
        placeObject(name, number, {x, y, center.z});
      }
    }
  }
}

void Silver::Line(const string name, int number, Vector3 start, Vector3 end) {
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
    placeObject(name, number, {x1, y1, start.z});
    if (x1 == x2 && y1 == y2)
      break;

    if (err * 2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (err * 2 < dx) {
      err += dx;
      y1 += sy;
    }
  }
}

void Silver::Oval(const string name, int number, Vector3 center,
                  Vector3 scale) {
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      if ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <=
          (scale.x * scale.x * scale.y * scale.y)) {
        placeObject(name, number, {x, y, center.z});
      }
    }
  }
}

void Silver::OvalHollow(const string name, int number, Vector3 center,
                        Vector3 scale) {
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      int first =
          ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <=
           (scale.x * scale.x * scale.y * scale.y));
      scale.x--;
      scale.y--;
      int second =
          ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <=
           (scale.x * scale.x * scale.y * scale.y));
      if (first && !second) {
        placeObject(name, number, {x, y, center.z});
      }
      scale.x++;
      scale.y++;
    }
  }
}

void Silver::CircleHollow(const string name, int number, Vector3 center,
                          int radius) {
  for (int y = center.y - radius; y <= center.y + radius; ++y) {
    for (int x = center.x - radius; x <= center.x + radius; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;
      int distanceSquared = dx * dx + dy * dy;

      if (distanceSquared >= (radius - 1) * (radius - 1) &&
          distanceSquared <= radius * radius) {
        placeObject(name, number, {x, y, center.z});
      }
    }
  }
}

void Silver::sprayRectangle(int spawns, Vector3 center, Vector3 scale,
                            string name, int number) {
  srand(static_cast<unsigned int>(time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    int offsetX = rand() % static_cast<int>(scale.x);
    int offsetY = rand() % static_cast<int>(scale.y);

    Vector3 position = {center.x + offsetX, center.y + offsetY, center.z};

    Rectangle(name, number, position, 1, 1);
  }
}

void Silver::sprayOval(int spawns, Vector3 center, Vector3 scale, string name,
                       int number) {
  srand(static_cast<unsigned int>(time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    double angle = static_cast<double>(rand()) / RAND_MAX * 2 * M_PI;
    double distanceX = (static_cast<double>(rand()) / RAND_MAX) * scale.x;
    double distanceY = (static_cast<double>(rand()) / RAND_MAX) * scale.y;

    Vector3 position = {center.x + static_cast<int>(distanceX * cos(angle)),
                        center.y + static_cast<int>(distanceY * sin(angle)),
                        center.z};

    Oval(name, number, position, scale);
  }
}

void Silver::spray(int spawns, Vector3 center, int range, string name,
                   int number) {
  srand(static_cast<unsigned int>(time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    int offsetX = rand() % (2 * range + 1) - range;
    int offsetY = rand() % (2 * range + 1) - range;

    Vector3 position = {center.x + offsetX, center.y + offsetY, center.z};

    placeObject(name, number, position);
  }
}

void Silver::sprayLine(int spawns, Vector3 start, Vector3 end, string name,
                       int number) {
  srand(static_cast<unsigned int>(time(nullptr)));

  int dx = end.x - start.x;
  int dy = end.y - start.y;
  int dz = end.z - start.z;

  for (int i = 0; i < spawns; ++i) {
    double t = static_cast<double>(rand()) / RAND_MAX;

    Vector3 position = {static_cast<int>(start.x + t * dx),
                        static_cast<int>(start.y + t * dy),
                        static_cast<int>(start.z + t * dz)};

    placeObject(name, number, position);
  }
}

void Silver::debug(const string message, const string mode) {

  char cwd[PATH_MAX];
  if (mode.empty())
    return;
  if (getcwd(cwd, sizeof(cwd)) != nullptr) {

    string icon_path = string(cwd) + "/";
    if (mode.at(0) == '/') {
      icon_path.clear();
    }
    if (mode == "w" || mode == "W") {
      icon_path += "Icons/warning.ico";
    } else if (mode == "q" || mode == "Q") {
      icon_path += "Icons/question.ico";
    } else if (mode == "s" || mode == "S") {
      icon_path += "Icons/subtraction.ico";
    } else if (mode == "p" || mode == "P" || mode == "d" || mode == "D") {
      icon_path += "Icons/positive.ico";
    } else if (mode == "e" || mode == "E") {
      icon_path += "Icons/error.ico";
    } else {
      icon_path += mode;
    }

    string command =
        "notify-send -i '" + icon_path + "' 'Debug Alarm' '" + message + "'";
    system(command.c_str());
  }
}

bool isValidName(const string name) {
  for (char c : name) {

    if (!isprint(c) || c == '\n' || c == '\r') {
      return false;
    }
  }
  return true;
}

void Silver::clean(Vector3 first, Vector3 second) {
  auto it = workspace.begin();
  while (it != workspace.end()) {
    Vector3 key = it->second.position;
    int x = key.x;
    int y = key.y;

    if (x >= min(first.x, second.x) && x <= max(first.x, second.x) &&
        y >= min(first.y, second.y) && y <= max(first.y, second.y)) {
      it = workspace.erase(it);
    } else {
      ++it;
    }
  }
}

void Silver::saveChunk(Vector3 range1, Vector3 range2, const string fileName,
                       const string mode) {
  ofstream fp;

  if (mode == "a") {
    fp.open(fileName, ios::app);
  } else {
    fp.open(fileName, ios::trunc);
  }

  if (!fp.is_open()) {
    Silver::debug("From \"void Silver::loadChunk(const string file)\" : Error "
                  "opening file.",
                  "e");
    return;
  }

  Vector3 camPos = Silver::Camera::CameraPos;
  Vector3 camScale = Silver::Camera::CameraScale;
  fp << "📷 " << camPos.x << " " << camPos.y << " " << camPos.z << " "
     << camScale.x << " " << camScale.y << " " << camScale.z << "\n";

  for (auto &objIt : workspace) {
    Vector3 pos = objIt.second.position;

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

void Silver::loadChunk(const string file) {
  ifstream fp(file);

  if (!fp.is_open()) {
    Silver::debug("From \"void Silver::loadChunk(const string file)\" Error: "
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

        Silver::Camera::setCam3(position, scale);
        type = 1;
      }
    } else if (buffer.rfind("🗺️", 0) == 0) {
      char context[5000];
      int coord1, coord2, coord3;
      if (sscanf(buffer.c_str() + sizeof("🗺️") - 1, " \"%[^\"]\" %d %d %d",
                 context, &coord1, &coord2, &coord3) == 4) {
        Silver::Drawing::draw(Vector3(coord1, coord2, coord3), context);
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
        Silver::placeObject(name, type, Vector3(coord1, coord2, coord3));
      }
    }
  }
}

void Silver::Camera::moveCamera(Vector3 V) {
  Silver::Camera::CameraPos.x += V.x;
  Silver::Camera::CameraPos.y += V.y;
}

void Silver::Camera::shakeCameraOnce(float intensity) {
  float offsetX =
      intensity * ((rand() % 100 / 100) * (rand() % 2 == 0 ? 1 : -1));
  float offsetY =
      intensity * ((rand() % 100 / 100) * (rand() % 2 == 0 ? 1 : -1));

  Silver::Camera::CameraPos.x += static_cast<int>(offsetX);
  Silver::Camera::CameraPos.y += static_cast<int>(offsetY);
}

void Silver::Camera::shakeCamera(float intensity, int shakes,
                                 float delayBetweenShakes) {
  float originalX = Silver::Camera::CameraPos.x;
  float originalY = Silver::Camera::CameraPos.y;

  for (int i = 0; i < shakes; ++i) {
    float offsetX = intensity * (rand() % 100 / 100 - 0);
    float offsetY = intensity * (rand() % 100 / 100 - 0);

    Silver::Camera::CameraPos.x = originalX + (int)offsetX;
    Silver::Camera::CameraPos.y = originalY + (int)offsetY;

    Silver::wait(delayBetweenShakes);
  }

  Silver::Camera::CameraPos.x = originalX;
  Silver::Camera::CameraPos.y = originalY;
}

pair<int, int> Silver::getConsoleSize() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  int width = w.ws_col;
  int height = w.ws_row;
  return {width, height};
}

Vector2 Silver::getConsoleCenter() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  int width = w.ws_col;
  int height = w.ws_row;
  return Vector2(width - width / 2, height - height / 2);
}

void Silver::Camera::zoomCamera(Vector3 v) {
  Silver::Camera::CameraScale.x += v.x;
  Silver::Camera::CameraScale.y += v.y;
  Silver::Camera::CameraScale.z += v.z;

  if (Silver::Camera::CameraScale.x < 0) {
    Silver::Camera::CameraScale.x =
        -Silver::Camera::CameraScale.x; // Flip the scale
    flipCamera(true, false); // Flip camera horizontally (adjust if needed)
  }
  if (Silver::Camera::CameraScale.y < 0) {
    Silver::Camera::CameraScale.y =
        -Silver::Camera::CameraScale.y; // Flip the scale
    flipCamera(false, true); // Flip camera vertically (adjust if needed)
  }
  if (Silver::Camera::CameraScale.z < 0) {
    Silver::Camera::CameraScale.z = -Silver::Camera::CameraScale.z;
  }
}

void startLoading() {
  Silver::clear();
  printf("--------------------------------\n");
  printf(". . . . . . . . . . . . . . . . \n");
  printf("--------------------------------\n");
  Silver::gotoxy(0, 1);
  cout << "#" << flush;
}

void mesh::setTransparence(bool value) { isTransparent = value; }

void mesh::translate2(Vector2 offset) {
  position.x += offset.x;
  position.y += offset.y;
}

void mesh::translate3(Vector3 offset) { position += offset; }

Vector3 mesh::getPos2() { return {position.x, position.y, 0}; }

Vector3 mesh::getPos3() { return position; }

void Silver::createPrefab(const string name, const string shape) {
  if (!isValidName(name)) {
    Silver::debug(
        "From \"void Silver::createObject(const string name, const string "
        "shape)\" : Invalid object name containing unprintable characters ",
        "e");
    return;
  }

  if (prefabrications.count(name) > 0) {
    return;
  }

  prefab A;
  A.name = name;
  A.shape = shape;

  prefabrications[name] = A;
}

void Silver::createEmptyPrefab(const string name) {
  if (prefabrications.count(name) > 0) {
    return;
  }

  prefab A;
  A.name = name;
  A.shape = "\0";
  A.isTransparent = true;
  prefabrications[name] = A;
}

int Silver::placeObject(const string objectName, int number, Vector3 position) {
  if (number < 0) {
    number = 0;
  }
  if (prefabrications.count(objectName) == 0) {
    return -1;
  }

  auto entry = prefabrications.find(objectName);
  if (entry != prefabrications.end()) {

    mesh X = entry->second;
    X.position = position;
    X.number = number;

    Silver::sprites_count++;

    int missingID = -1;
    if (Silver::sprites_count != numeric_limits<int>::max()) {
      missingID = Silver::sprites_count++;
    } else {
      for (int id = 0; id < numeric_limits<int>::max(); ++id) {

        if (workspace.find(id) == workspace.end()) {
          missingID = id;
          break;
        }
      }
    }

    if (missingID == -1) {
      Silver::debug("Maximum Object Reached", "e");
      return -1;
    }

    workspace.insert({missingID, X});
    return missingID;
  }

  return -1;
}

int Silver::UI::placeUI(const string objectName, int number, Vector3 position) {
  if (number < 0) {
    number = 0;
  }
  if (prefabrications.count(objectName) == 0) {
    return -1;
  }

  auto entry = prefabrications.find(objectName);
  if (entry != prefabrications.end()) {
    mesh X = entry->second;
    X.position = position;
    X.number = number;
    X.comp.isUI = true;

    Silver::sprites_count++;

    int missingID = -1;
    if (Silver::sprites_count != numeric_limits<int>::max()) {
      missingID = Silver::sprites_count++;
    } else {
      for (int id = 0; id < numeric_limits<int>::max(); ++id) {

        if (workspace.find(id) == workspace.end()) {
          missingID = id;
          break;
        }
      }
    }

    if (missingID == -1) {
      Silver::debug("Maximum Object Reached", "e");
      return -1;
    }

    workspace.insert({missingID, X});
    return missingID;
  }

  return -1;
}

void Silver::UI::changeToUI(variant<vector<int>, int> workspaceIDs) {
  auto setUI = [](int id) { workspace[id].comp.isUI = true; };

  if (holds_alternative<int>(workspaceIDs)) {
    setUI(get<int>(workspaceIDs));
  } else {
    for (int id : get<vector<int>>(workspaceIDs)) {
      setUI(id);
    }
  }
}

int Silver::unoccupiedNumber(const string objectName) {
  unordered_set<int> occupiedNumbers;

  for (const auto &objIt : workspace) {
    if (objIt.second.name == objectName) {
      occupiedNumbers.insert(objIt.second.number);
    }
  }

  int min = 0;
  while (occupiedNumbers.find(min) != occupiedNumbers.end()) {
    min++;
  }

  return min;
}

int Silver::addObject(const string objectName, Vector3 position) {
  return placeObject(objectName, Silver::unoccupiedNumber(objectName),
                     position);
}

int Silver::put(const string objectName, Vector3 position) {
  return placeObject(objectName, 0, position);
}

atomic<bool> isRunningCam(false);
thread videoThread;
mutex camMutex;

void Silver::Camera::startVideo(float FPS) {
  if (isRunningCam)
    return; // Prevent multiple threads running

  isRunningCam = true;

  videoThread = thread([FPS]() {
    while (isRunningCam) {
      camMutex.lock();
      Silver::Camera::printCam(); // Assuming this function prints the camera
                                  // info
      camMutex.unlock();

      this_thread::sleep_for(chrono::milliseconds(
          int(1000 / FPS))); // Wait for next frame based on FPS
    }
  });
}

void Silver::hold() {
  while (1)
    wait(2147483647);
}

void Silver::Camera::endVideo() {
  {
    lock_guard<mutex> lock(camMutex);
    if (!isRunningCam)
      return;

    isRunningCam = false;
  }
  if (videoThread.joinable()) {
    videoThread.join();
  }
}

void Silver::Camera::restartVideo(float FPS) {
  endVideo();
  startVideo(FPS);
}
void Silver::wait(int time) {
  this_thread::sleep_for(chrono::milliseconds(static_cast<int>(time)));
}

vector<int> Silver::getObjectsAt3(Vector3 pos) {
  vector<int> ids;

  for (auto &entry : workspace) {
    if (entry.second.position.x == pos.x && entry.second.position.y == pos.y &&
        entry.second.position.z == pos.z) {

      ids.push_back(entry.first);
    }
  }

  return ids;
}
vector<int> Silver::getObjectsAt2(Vector2 pos) {
  vector<int> ids;

  for (auto &entry : workspace) {
    if (entry.second.position.x == pos.x && entry.second.position.y == pos.y) {

      ids.push_back(entry.first);
    }
  }

  return ids;
}

void Silver::setObjectXY(const variant<int, vector<int>> objectID,
                         Vector2 pos) {
  int x = pos.x;
  int y = pos.y;

  vector<int> idsToUpdate;

  if (holds_alternative<int>(objectID)) {
    int singleID = get<int>(objectID);
    if (singleID == all_numbers) {
      for (auto &entry : workspace) {
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
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh &X = it->second;
      int z = X.position.z;

      X.position = {x, y, z};
    }
  }
}
void Silver::setObjectY(const variant<int, vector<int>> objectID, Vector3 pos) {
  int y = pos.y;

  vector<int> idsToUpdate;

  if (holds_alternative<int>(objectID)) {
    int singleID = get<int>(objectID);
    if (singleID == all_numbers) {
      for (auto &entry : workspace) {
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
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh &X = it->second;
      int z = X.position.z;
      int x = X.position.x;
      X.position = {x, y, z};
    }
  }
}
void Silver::setObjectX(const variant<int, vector<int>> objectID, Vector3 pos) {
  int x = pos.x;

  vector<int> idsToUpdate;

  if (holds_alternative<int>(objectID)) {
    int singleID = get<int>(objectID);
    if (singleID == all_numbers) {
      for (auto &entry : workspace) {
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
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh &X = it->second;
      int y = X.position.y;
      int z = X.position.z;

      X.position = {x, y, z};
    }
  }
}

void Silver::setObjectRandom(const variant<int, vector<int>> objectID,
                             const pair<int, int> &xRange,
                             const pair<int, int> &yRange) {
  vector<int> idsToUpdate;

  if (holds_alternative<int>(objectID)) {
    int singleID = get<int>(objectID);
    if (singleID == all_numbers) {
      for (auto &entry : workspace) {
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
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh &X = it->second;
      int z = X.position.z;

      int x = (xRange.first == xRange.second)
                  ? xRange.first
                  : getRandom(xRange.first, xRange.second);
      int y = (yRange.first == yRange.second)
                  ? yRange.first
                  : getRandom(yRange.first, yRange.second);

      X.position = {x, y, z};
    }
  }
}

void Silver::setObjectPositionToTarget(
    const variant<int, vector<int>> objectIDs, int spriteID) {

  Vector3 targetPos = Silver::getMeshValue(spriteID).position;

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
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh &objMesh = it->second;
      objMesh.position = targetPos;
    }
  }
}
void Silver::glideObjectPositionToTarget(
    const variant<int, vector<int>> objectIDs, int spriteID, float speed) {

  Vector3 targetPos = Silver::getMeshValue(spriteID).position;

  int maxSteps = 0;
  vector<tuple<int, int, int>> idsToUpdate;

  if (holds_alternative<int>(objectIDs)) {
    int singleID = get<int>(objectIDs);

    Vector3 currentPos = Silver::getMeshValue(singleID).position;
    int x_steps = abs(targetPos.x - currentPos.x);
    int y_steps = abs(targetPos.y - currentPos.y);
    maxSteps = max({maxSteps, x_steps, y_steps});

    idsToUpdate.emplace_back(singleID, x_steps, y_steps);
  } else {
    const vector<int> &idList = get<vector<int>>(objectIDs);

    for (int id : idList) {
      Vector3 currentPos = Silver::getMeshValue(id).position;
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
    Silver::wait(stepDuration);

    for (const auto &[id, x_steps, y_steps] : idsToUpdate) {
      Vector3 currentPos = Silver::getMeshValue(id).position;

      if (i < x_steps && (i % (maxSteps / max(1, x_steps)) == 0)) {
        int dirX = (targetPos.x > currentPos.x) ? 1 : -1;
        Silver::moveObjectX(id, dirX);
      }

      if (i < y_steps && (i % (maxSteps / max(1, y_steps)) == 0)) {
        int dirY = (targetPos.y > currentPos.y) ? 1 : -1;
        Silver::moveObjectY(id, dirY);
      }
    }
  }
}

void Silver::glideObjectXY(const variant<int, vector<int>> &ids, Vector2 offset,
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

    Vector3 current_pos = Silver::getMeshValue(singleId).position;

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

      Vector3 current_pos = Silver::getMeshValue(id).position;

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
    Silver::wait(stepDuration);

    for (const auto &[id, stepsX] : idsToUpdateX) {
      if (i < stepsX && (i % (maxSteps / stepsX) == 0)) {
        int dirX = (offset.x > 0) ? 1 : -1;
        Silver::moveObjectX(id, dirX);
      }
    }

    for (const auto &[id, stepsY] : idsToUpdateY) {
      if (i < stepsY && (i % (maxSteps / stepsY) == 0)) {
        int dirY = (offset.y > 0) ? 1 : -1;
        Silver::moveObjectY(id, dirY);
      }
    }
  }
}

int Silver::getRandom(int min, int max) {
  static mt19937 rng(random_device{}() ^
                     chrono::system_clock::now().time_since_epoch().count());
  uniform_int_distribution<int> dist(min, max);
  return dist(rng);
}

void Silver::setObjectPosition(const variant<int, vector<int>> objectID,
                               Vector3 pos) {
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
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh &X = it->second;
      X.position = {x, y, z};
    }
  }
}

mutex coutMutex;
void Silver::Particle::applyParticleComponent(
    int objID, const string objectName, Vector3 pos, int radius, int lifeTime,
    double speed, int quantity, int particleLifetime) {

  auto particles = make_shared<vector<int>>();
  auto particleMutex = make_shared<mutex>();

  thread spawnThread([=]() {
    auto startTime = chrono::steady_clock::now();
    bool running = true;

    int spawnDelay = 1000 / quantity + Silver::getRandom(-10, 10);
    if (spawnDelay < 10)
      spawnDelay = 10;

    while (running) {

      if (!Silver::isAlive(objID)) {
        running = false;
        break;
      }

      int distance = Silver::getRandom(0, radius);
      double randomAngle = Silver::getRandom(0, 360);
      int offsetX = static_cast<int>(distance * cos(randomAngle * M_PI / 180));
      int offsetY = static_cast<int>(distance * sin(randomAngle * M_PI / 180));

      Vector2 targetPos = {pos.x + offsetX, pos.y + offsetY};
      int particleID = Silver::placeObject(objectName, 0, pos);

      {
        lock_guard<mutex> lock(*particleMutex);
        particles->push_back(particleID);
      }

      thread([particleID, targetPos, speed, particleLifetime]() {
        Silver::glideObjectXY(particleID, targetPos, speed);
        Silver::wait(particleLifetime);
        Silver::destroy(particleID);
      }).detach();

      if (lifeTime != -1) {
        auto elapsed = chrono::steady_clock::now() - startTime;
        int elapsedTime =
            chrono::duration_cast<chrono::milliseconds>(elapsed).count();
        if (elapsedTime >= lifeTime) {
          running = false;
        }
      }

      this_thread::sleep_for(chrono::milliseconds(spawnDelay));
    }

    {
      lock_guard<mutex> lock(*particleMutex);
      for (int particleID : *particles) {
        if (Silver::isAlive(particleID)) {
          Silver::destroy(particleID);
        }
      }
    }
  });

  spawnThread.detach();
}

void Silver::moveObjectXY(const variant<int, vector<int>> objectID,
                          Vector2 pos) {
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
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh &X = it->second;
      X.position += Vector3(dx, dy, 0);
      int x = X.position.x;
      int y = X.position.y;
      int z = X.position.z;
    }
  }
}

void Silver::moveObjectPosition(const variant<int, vector<int>> objectID,
                                Vector2 pos) {
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
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh &X = it->second;
      X.position += Vector2(dx, dy);
      int x = X.position.x;
      int y = X.position.y;
      int z = X.position.z;
    }
  }
}

void Silver::moveObjectX(const variant<int, vector<int>> objectID,
                         int x_offset) {
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
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh &X = it->second;
      X.position += Vector3(dx, 0, 0);
      int x = X.position.x;
      int y = X.position.y;
      int z = X.position.z;
    }
  }
}

void Silver::moveObjectY(const variant<int, vector<int>> objectID,
                         int y_offset) {
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
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh &X = it->second;
      X.position += Vector3(0, dy, 0);
      int x = X.position.x;
      int y = X.position.y;
      int z = X.position.z;
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
    Vector3 current_pos = Silver::getMeshValue(singleId).position;
    int targetPos = setPosition ? offset
                                : (isYMovement ? current_pos.y + offset
                                               : current_pos.x + offset);
    int steps = abs(targetPos - (isYMovement ? current_pos.y : current_pos.x));
    maxSteps = std::max(maxSteps, steps);
    idsToUpdate.emplace_back(singleId, steps);
  } else {
    const vector<int> &idList = get<vector<int>>(ids);
    for (const auto &id : idList) {
      Vector3 current_pos = Silver::getMeshValue(id).position;
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
        Silver::moveObjectY(id, dir);
      } else {
        int dir = (offset > 0) ? 1 : -1;
        Silver::moveObjectX(id, dir);
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
void Silver::glideObjectY(const variant<int, vector<int>> &ids, int y_offset,
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
void Silver::glideObjectX(const variant<int, vector<int>> &ids, int x_offset,
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

int animationID = 0;

void Silver::Animation::stopAnimation(variant<int, vector<int>> objID,
                                      bool reset) {
  vector<int> ids;
  if (holds_alternative<int>(objID)) {
    ids.push_back(get<int>(objID));
  } else {
    ids = get<vector<int>>(objID);
  }

  for (int id : ids) {
    workspace[id].comp.playingID = -1;
    if (reset) {
      workspace[id].shape = workspace[id].comp.animationBuffer;
    }
  }
}

void Silver::Animation::applyAnimation(variant<int, vector<int>> objID,
                                       const string filename) {
  ifstream file(filename);
  if (!file) {
    cerr << "Error opening file: " << filename << endl;
    return;
  }

  vector<string> animationFrames;
  float fps = -1;
  int transition = -1;

  string line;
  while (getline(file, line)) {
    if (line.compare(0, 6, "$write") == 0) {
      string frames = line.substr(7);
      stringstream ss(frames);
      string frame;
      while (getline(ss, frame, ',')) {
        animationFrames.push_back(frame);
      }
    } else if (line.compare(0, 4, "$for") == 0) {
      string command = line.substr(5);
      stringstream ss(command);
      int repeatCount;
      string frame;
      if (ss >> repeatCount >> frame) {
        for (int i = 0; i < repeatCount; ++i) {
          animationFrames.push_back(frame);
        }
      }
    } else if (line.compare(0, 4, "FPS ") == 0) {
      string fpsStr = line.substr(4);
      fps = stof(fpsStr);
    } else if (line.compare(0, 12, "TRANSITION ") == 0) {
      string transitionStr = line.substr(12);
      if (transitionStr == "IMMIDIATE") {
        transition = -1;
      } else {
        transition = stoi(transitionStr);
      }
    }
  }

  applyAnimationRaw(objID, animationFrames, fps, transition);
}

void Silver::Animation::applyAnimationRaw(variant<int, vector<int>> objID,
                                          vector<string> animation, float fps,
                                          int transition) {
  static int animationID =
      0; // Make animationID static since there's no class instance

  vector<int> ids;
  if (holds_alternative<int>(objID)) {
    ids.push_back(get<int>(objID));
  } else {
    ids = get<vector<int>>(objID);
  }

  int newAnimationID = animationID++;

  auto animationThread = [ids, animation, fps, newAnimationID,
                          transition]() mutable {
    int frameIndex = 0;
    int totalFrames = animation.size();

    for (int id : ids) {
      stopAnimation(
          id, false); // Ensure stopAnimation is a free function or accessible
      workspace[id].comp.playingID = newAnimationID;
      workspace[id].comp.animationBuffer = workspace[id].shape;
    }

    while (!ids.empty()) {
      string currentFrame = animation[frameIndex];

      for (auto it = ids.begin(); it != ids.end();) {
        int id = *it;

        if (workspace[id].comp.playingID == newAnimationID) {
          if (transition == -1 || frameIndex < transition) {
            workspace[id].shape = currentFrame;
          } else if (frameIndex == transition) {
            workspace[id].shape = currentFrame;
          }
          ++it;
        } else {
          it = ids.erase(it);
        }
      }

      Silver::wait(
          static_cast<int>(1000 / fps)); // Silver::wait must be accessible

      frameIndex = (frameIndex + 1) % totalFrames;
    }

    for (int id : ids) {
      workspace[id].comp.playingID = -1;
      workspace[id].shape = workspace[id].comp.animationBuffer;
    }
  };

  thread(animationThread).detach();
}

void Silver::glideObjectRandom(const variant<int, vector<int>> &ids,
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

    Vector3 current_pos = Silver::getMeshValue(singleId).position;
    int targetX = getRandom(xRange.first, xRange.second);
    int targetY = getRandom(yRange.first, yRange.second);

    int x_steps = abs(targetX - current_pos.x);
    int y_steps = abs(targetY - current_pos.y);
    maxSteps = max({maxSteps, x_steps, y_steps});

    idsToUpdate.emplace_back(singleId, x_steps, y_steps,
                             Vector3{targetX, targetY, current_pos.z});
  } else {
    const vector<int> &idList = get<vector<int>>(ids);
    for (const auto &id : idList) {

      Vector3 current_pos = Silver::getMeshValue(id).position;
      int targetX = getRandom(xRange.first, xRange.second);
      int targetY = getRandom(yRange.first, yRange.second);

      int x_steps = abs(targetX - current_pos.x);
      int y_steps = abs(targetY - current_pos.y);
      maxSteps = max({maxSteps, x_steps, y_steps});

      idsToUpdate.emplace_back(id, x_steps, y_steps,
                               Vector3{targetX, targetY, current_pos.z});
    }
  }

  float stepDuration = speed / maxSteps;

  for (int i = 0; i < maxSteps; ++i) {
    Silver::wait(stepDuration);

    for (const auto &[id, x_steps, y_steps, target_pos] : idsToUpdate) {
      Vector3 current_pos = Silver::getMeshValue(id).position;

      if (i < x_steps && (i % (maxSteps / x_steps) == 0)) {
        int dirX = (target_pos.x > current_pos.x) ? 1 : -1;
        Silver::moveObjectX(id, dirX);
      }

      if (i < y_steps && (i % (maxSteps / y_steps) == 0)) {
        int dirY = (target_pos.y > current_pos.y) ? 1 : -1;
        Silver::moveObjectY(id, dirY);
      }
    }
  }
}

void Silver::setRawMode(bool value) {
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

vector<Vector3> Silver::getOvalPoints(Vector3 center, Vector3 scale) {
  vector<Vector3> points;
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;
      if ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <=
          (scale.x * scale.x * scale.y * scale.y)) {
        points.push_back({x, y, center.z});
      }
    }
  }
  return points;
}

vector<Vector3> Silver::getOvalHollowPoints(Vector3 center, Vector3 scale) {
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
        points.push_back({x, y, center.z});
      }
    }
  }
  return points;
}

vector<Vector3> Silver::getLinePoints(Vector3 start, Vector3 end) {
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
    points.push_back({x1, y1, start.z});
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

vector<Vector3> Silver::getRectanglePoints(Vector3 topLeft, int width,
                                           int height) {
  vector<Vector3> points;
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      points.push_back({topLeft.x + i, topLeft.y + j, topLeft.z});
    }
  }
  return points;
}
vector<Vector3> Silver::getRectangleHollowPoints(Vector3 topLeft, int width,
                                                 int height) {
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

Vector3 Silver::getLocation(int id) {
  auto it = workspace.find(id);
  if (it != workspace.end()) {
    return workspace[id].position;
  }
  return {-1, -1, -1};
}

double Silver::calculateDistance3(Vector3 first, Vector3 second) {
  float dx = first.x - second.x;
  float dy = first.y - second.y;
  float dz = first.z - second.z;
  return sqrt(dx * dx + dy * dy + dz * dz);
}

double Silver::calculateDistance2(Vector2 first, Vector2 second) {
  float dx = first.x - second.x;
  float dy = first.y - second.y;
  return sqrt(dx * dx + dy * dy);
}

bool Silver::isAlive(int obj) { return workspace.find(obj) != workspace.end(); }

void Silver::applyFunction(const vector<int> &ids, function<void(int)> func,
                           char mode, ...) {
  vector<int> targetObjects;
  int key = 0;
  Vector3 pos = {0, 0, 0};
  va_list args;
  va_start(args, mode);

  if (mode == 'k') {
    key = va_arg(args, int);
  } else if (mode == 'm') {
    key = Silver::Mouse::mouseKey;
  }
  va_end(args);

  for (int obj : ids) {
    lock_guard<mutex> lock(Silver::thread_mutex);

    if (mode == 'h')
      pos = va_arg(args, Vector3);
    if (mode == 'H')
      pos = Silver::Camera::getScreenPosition(Silver::getLocation(obj));
    if (mode == 'c')
      pos = va_arg(args, Vector3);
    if (mode == 'C')
      pos = Silver::Camera::getScreenPosition(Silver::getLocation(obj));
    Silver::activeThreads.emplace_back([obj, func, mode, key, pos]() {
      while (Silver::isAlive(obj)) {
        if (mode == 'k' || mode == 'm' || mode == 'c' || mode == 'C') {
          while (true) {
            Silver::Keyboard::getKey();
            if (Silver::Keyboard::isKey(key)) {
              if (mode == 'c' || mode == 'C') {
                Vector3 cursorPosition = {Silver::Mouse::cursorPositionX,
                                          Silver::Mouse::cursorPositionY, 0};
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
            Vector3 cursorPosition = {Silver::Mouse::cursorPositionX,
                                      Silver::Mouse::cursorPositionY, 0};
            if (cursorPosition == pos) {
              break;
            }
          }
        }
        func(obj);
      }
    });
  }
}

void Silver::applyTag(const vector<int> &ids, const string tag) {
  for (int id : ids) {
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      it->second.tags.push_back(tag);
    }
  }
}

vector<int> Silver::seekTag(const string tag) {
  vector<int> result;

  for (const auto &entry : workspace) {
    if (find(entry.second.tags.begin(), entry.second.tags.end(), tag) !=
        entry.second.tags.end()) {
      result.emplace_back(entry.first);
    }
  }

  return result;
}

vector<string> Silver::getTag(int id) {
  auto it = workspace.find(id);
  if (it != workspace.end()) {
    return it->second.tags;
  }

  return {};
}

mutex workspaceMutex;
atomic<bool> stopGlobal{false};
unordered_map<int, thread> cullingThreads;

unordered_map<int, thread> flowThreads;

void globalCullingThread(int rootId) {
  int lastProcessedDepth = 0;
  while (!stopGlobal) {
    lock_guard<mutex> lock(workspaceMutex);

    auto it = workspace.begin();
    while (it != workspace.end()) {
      auto &m = it->second;

      if (m.comp.fluidRoot != rootId && it->first != rootId) {
        ++it;
        continue;
      }

      int parentId = m.comp.fluidParent;
      queue<int> ancestorsQueue;
      ancestorsQueue.push(parentId);
      bool hasDeadAncestor = false;

      while (!ancestorsQueue.empty()) {
        int currentParentId = ancestorsQueue.front();
        ancestorsQueue.pop();

        if (currentParentId == -1)
          break;

        auto parentIt = workspace.find(currentParentId);
        if (parentIt == workspace.end() || parentIt->second.comp.isDead) {
          hasDeadAncestor = true;
          break;
        }

        ancestorsQueue.push(parentIt->second.comp.fluidParent);
      }

      if (hasDeadAncestor && !m.comp.isDead) {
        m.comp.isDead = true;
      }

      int depth = m.comp.fluidDepth;

      for (const auto &[otherId, otherMesh] : workspace) {
        if (otherId == it->first)
          continue;
        if (otherMesh.position.x == m.position.x &&
            otherMesh.position.y == m.position.y &&
            otherMesh.comp.preventFlowing) {
          if (!m.comp.isDead) {
            m.comp.isDead = true;
          }
          break;
        }
      }

      if (m.comp.isDead) {
        if (lastProcessedDepth != depth)
          Silver::wait(m.comp.drySpeed);
        it = workspace.erase(it);
        lastProcessedDepth = depth;
        continue;
      }

      ++it;
    }
  }
}

void startGlobalCulling(int rootId) {
  stopGlobal = false;
  if (cullingThreads.find(rootId) == cullingThreads.end()) {
    cullingThreads[rootId] = thread(globalCullingThread, rootId);
  }
}

void threadedFlow(int rootId) {
  queue<pair<int, int>> flowQueue;
  {
    lock_guard<mutex> lock(workspaceMutex);
    if (workspace.find(rootId) == workspace.end() ||
        !workspace[rootId].comp.isFluid)
      return;
    flowQueue.push({rootId, 0});
  }

  const double waitTimeReductionFactor = 0.9;
  int lastGen = -1;

  while (!flowQueue.empty() && !stopGlobal) {
    auto [currentId, generationCount] = flowQueue.front();
    flowQueue.pop();

    {
      lock_guard<mutex> lock(workspaceMutex);
      if (workspace.find(currentId) == workspace.end() ||
          workspace[currentId].comp.isDead)
        continue;
    }

    if (generationCount >= workspace[currentId].comp.maximumDistance)
      break;

    double waitTime = workspace[currentId].comp.diffusionSpeed *
                      pow(waitTimeReductionFactor, generationCount);
    if (generationCount != lastGen) {
      Silver::wait(waitTime);
      lastGen = generationCount;
    }

    bool flowed = false;

    for (int dir = 0; dir < 4; ++dir) {
      int newX = workspace[currentId].position.x;
      int newY = workspace[currentId].position.y;

      switch (dir) {
      case 0:
        newY++;
        break;
      case 1:
        newY--;
        break;
      case 2:
        newX--;
        break;
      case 3:
        newX++;
        break;
      }

      bool canFlow = true;
      {
        lock_guard<mutex> lock(workspaceMutex);
        for (const auto &[neighborId, neighborMesh] : workspace) {
          if (neighborMesh.position.x == newX &&
              neighborMesh.position.y == newY) {
            if (neighborMesh.comp.preventFlowing || neighborMesh.comp.isFluid) {
              canFlow = false;
              break;
            }
          }
        }
      }

      if (canFlow) {
        int newDupId;
        {
          lock_guard<mutex> lock(workspaceMutex);
          newDupId = Silver::duplicate(currentId)[0];
          workspace[newDupId].position.x = newX;
          workspace[newDupId].position.y = newY;
          workspace[newDupId].comp.fluidParent = rootId;
          workspace[newDupId].comp.fluidRoot = rootId;
          workspace[newDupId].comp.fluidDepth =
              workspace[currentId].comp.fluidDepth + 1;
        }

        flowQueue.push({newDupId, generationCount + 1});
        flowed = true;
      }
    }

    if (!flowed)
      break;
  }
}

void startFlow(int rootId) {
  if (flowThreads.find(rootId) == flowThreads.end()) {
    flowThreads[rootId] = thread(threadedFlow, rootId);
  }
}

void stopAllFlowAndCulling() {
  stopGlobal = true;

  for (auto &[id, thread] : flowThreads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  flowThreads.clear();

  for (auto &[id, thread] : cullingThreads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  cullingThreads.clear();
}

void flow(int rootId) {
  startFlow(rootId);
  startGlobalCulling(rootId);
}

void Silver::Fluid::Liquify(const variant<int, vector<int>> &IDs,
                            double diffusionSpeed, int maximumDistance) {

  auto liquifyMesh = [&](int id) {
    if (workspace.find(id) != workspace.end()) {
      mesh &m = workspace[id];
      m.comp.isFluid = true;
      m.comp.diffusionSpeed = diffusionSpeed;
      m.comp.maximumDistance = maximumDistance;
      flow(id);
    }
  };

  if (holds_alternative<int>(IDs)) {
    int id = get<int>(IDs);
    liquifyMesh(id);
  } else if (holds_alternative<vector<int>>(IDs)) {
    for (int id : get<vector<int>>(IDs)) {
      liquifyMesh(id);
    }
  }
}

vector<int> Silver::duplicate(const variant<int, vector<int>> &IDs) {
  vector<int> duplicatedIDs;

  if (holds_alternative<int>(IDs)) {
    int id = get<int>(IDs);

    Silver::sprites_count++;
    workspace[sprites_count - 1] = workspace[id];
    duplicatedIDs.push_back(sprites_count - 1);

  } else if (holds_alternative<vector<int>>(IDs)) {

    for (int id : get<vector<int>>(IDs)) {
      Silver::sprites_count++;
      workspace[sprites_count - 1] = workspace[id];
      duplicatedIDs.push_back(sprites_count - 1);
    }
  }

  return duplicatedIDs;
}

void Silver::Fluid::Unliquify(const variant<int, vector<int>> &IDs) {
  auto unliquifyMesh = [&](int id) {
    if (workspace.find(id) != workspace.end()) {
      mesh &m = workspace[id];
      m.comp.isFluid = false;
    }
  };

  if (holds_alternative<int>(IDs)) {
    int id = get<int>(IDs);
    unliquifyMesh(id);
  } else if (holds_alternative<vector<int>>(IDs)) {
    for (int id : get<vector<int>>(IDs)) {
      unliquifyMesh(id);
    }
  }
}
void Silver::Fluid::Unsolidify(const variant<int, vector<int>> &IDs) {
  auto unsolidifyMesh = [&](int id) {
    if (workspace.find(id) != workspace.end()) {
      mesh &m = workspace[id];
      m.comp.preventFlowing = false;
    }
  };

  if (holds_alternative<int>(IDs)) {
    int id = get<int>(IDs);
    unsolidifyMesh(id);
  } else if (holds_alternative<vector<int>>(IDs)) {
    for (int id : get<vector<int>>(IDs)) {
      unsolidifyMesh(id);
    }
  }
}
void Silver::Fluid::Solidify(const variant<int, vector<int>> &IDs) {
  auto solidifyMesh = [&](int id) {
    if (workspace.find(id) != workspace.end()) {
      mesh &m = workspace[id];
      m.comp.preventFlowing = true;
    }
  };

  if (holds_alternative<int>(IDs)) {
    int id = get<int>(IDs);
    solidifyMesh(id);
  } else if (holds_alternative<vector<int>>(IDs)) {
    for (int id : get<vector<int>>(IDs)) {
      solidifyMesh(id);
    }
  }
}

zone Silver::buildZone(
    variant<tuple<int, int, int, int>, vector<tuple<int, int, int, int>>>
        shape) {
  return zone(shape);
}

bool Silver::isInZone(const zone &z, Vector3 pos) {
  int pointX = pos.x, pointY = pos.y;
  if (holds_alternative<tuple<int, int, int, int>>(z.shape)) {
    const auto &rect = get<tuple<int, int, int, int>>(z.shape);
    int x = get<0>(rect);
    int y = get<1>(rect);
    int width = get<2>(rect);
    int height = get<3>(rect);

    return (pointX >= x && pointX <= x + width) &&
           (pointY >= y && pointY <= y + height);
  } else {
    const auto &rects = get<vector<tuple<int, int, int, int>>>(z.shape);
    for (const auto &rect : rects) {
      int x = get<0>(rect);
      int y = get<1>(rect);
      int width = get<2>(rect);
      int height = get<3>(rect);

      if ((pointX >= x && pointX <= x + width) &&
          (pointY >= y && pointY <= y + height)) {
        return true;
      }
    }
  }
  return false;
}
