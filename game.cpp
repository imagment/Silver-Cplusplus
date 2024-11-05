#include <iostream>

#include <thread>

#include <vector>

#include <string>

#include <cmath>

#include <cstdio>

#include <fstream>

#include <map>

#include <stdexcept>

#include <csignal>

#include <sstream>

#include <atomic>

#include <cstdarg>

#include <ctime>

#include <string>

#include <cstdlib>

#include <sys/ioctl.h>

#include <tuple>

#include <unordered_map>

#include <unordered_set>

#include <set>

#include <functional>

#include <termios.h>

#include <fcntl.h>

#include <unistd.h>

#include <typeinfo>

#include <condition_variable>

#include "game.hpp"

#define BUFFER_SIZE 5000

using namespace std;

mutex scriptMutex;
unordered_set < string > currentPressedKeys;
unordered_set < string > previousPressedKeys;
atomic < bool > keepListening(true);

mutex keyMutex;
World workspace;
World killedSprites;
World empty;
map <
  const string, objects > prefabs;

vector < scene > scenes;

string out_of_bounds = "🟦";
string null_rep = "#";

int WorldX = 20, WorldY = 20;

Silver silver;
void Silver::Scene::saveWorldAs(std::string name) {

  scenes[name] = workspace;

}

void Silver::Scene::changeSceneTo(std::string name) {
  auto it = scenes.find(name);
  if (it != scenes.end()) {
    workspace = it -> second;
  }
}

void Silver::Scene::resetScene() {
  World empty;
  workspace = empty;
}

void Silver::Camera::cell(int c) {
  silver.camera.cellSize = c;
}

void Silver::Camera::configCameraException(string o, string n) {
  null_rep = n;
  out_of_bounds = o;
}

vector < mesh > Silver::MeshAt3(Vec3 pos) {
  vector < mesh > result;

  for (auto & entry: workspace) {
    if (entry.second.object.position == pos) result.push_back(entry.second);
  }

  return result;
}

vector < mesh > Silver::MeshAt2(Vec3 pos) {
  vector < mesh > result;

  for (auto & entry: workspace) {
    if (entry.second.object.position.x == pos.x && entry.second.object.position.y == pos.y) result.push_back(entry.second);
  }

  return result;
}

int Silver::PlaceMesh(mesh m, Vec3 pos) {
  workspace.insert({
    {
      sprites_count++
    },
    m
  });
  return sprites_count - 1;
}

mesh * Silver::GetObject(int objID) {

  return & workspace[objID];

}

mesh Silver::GetMeshOf(int objID) {

  return workspace[objID];

}
map < char, int > keys;

char Silver::Keyboard::getKey() {
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, & oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, & newt);

  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, & oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    if (ch >= 'a' && ch <= 'z') ch += 'A' - 'a';
    keyBuffer = ch;
    return ch;
  }
  keyBuffer = '\0';
  return '\0';
}

bool Silver::Keyboard::isKey(char key) {
  if (keyBuffer == key) {
    return true;
  }
  return false;
}

bool Silver::Mouse::isMouse() {
  if (silver.keyboard.keyBuffer == mouseKey) {
    return true;
  }
  return false;
}

void Silver::gotoxy(int x, int y) {
  cout << "\033[" << y + 1 << ";" << x + 1 << "H" << flush;
}

Vec3 rotatePoint(Vec3 point, float angle) {
  float radians = angle * (M_PI / 180.f);
  float cosA = cos(radians);
  float sinA = sin(radians);
  return Vec3(static_cast < int > (round(point.x * cosA - point.y * sinA)),
    static_cast < int > (round(point.x * sinA + point.y * cosA)), 0);
}

void Silver::Drawing::draw(Vec3 pos, string c) {
  mesh p;
  p.object.position = pos;
  p.object.number = -214743648;
  p.shape = c;
  p.transparency = 0;

  workspace.insert({
    -2147473648,
    p
  });
}

void Silver::Drawing::spray(int spawns, Vec3 center, int range, string c) {
  srand(static_cast < unsigned int > (time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    int offsetX = rand() % (2 * range + 1) - range;
    int offsetY = rand() % (2 * range + 1) - range;

    Vec3 position = {
      center.x + offsetX,
      center.y + offsetY,
      center.z
    };

    draw(position, c);
  }
}
void Silver::Drawing::sprayOval(int spawns, Vec3 center, Vec3 scale, string c) {
  srand(static_cast < unsigned int > (time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    double angle = static_cast < double > (rand()) / RAND_MAX * 2 * M_PI;
    double distanceX = (static_cast < double > (rand()) / RAND_MAX) * scale.x;
    double distanceY = (static_cast < double > (rand()) / RAND_MAX) * scale.y;

    Vec3 position = {
      center.x + static_cast < int > (distanceX * cos(angle)),
      center.y + static_cast < int > (distanceY * sin(angle)),
      center.z
    };

    draw(position, c);
  }
}

void Silver::Drawing::sprayRectangle(int spawns, Vec3 center, Vec3 scale, string c) {
  srand(static_cast < unsigned int > (time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    int offsetX = rand() % static_cast < int > (scale.x);
    int offsetY = rand() % static_cast < int > (scale.y);

    Vec3 position = {
      center.x + offsetX,
      center.y + offsetY,
      center.z
    };

    draw(position, c);
  }
}

void Silver::Drawing::Rectangle(Vec3 topLeft, int width, int height, string c) {
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      Vec3 pos = {
        topLeft.x + i,
        topLeft.y + j,
        topLeft.z
      };
      draw(pos, c);
    }
  }
}
void Silver::sprayRectangle(string name, int number, int spawns, Vec3 center, Vec3 scale) {
  srand(static_cast < unsigned int > (time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    int offsetX = rand() % static_cast < int > (scale.x);
    int offsetY = rand() % static_cast < int > (scale.y);

    Vec3 position = {
      center.x + offsetX,
      center.y + offsetY,
      center.z
    };

    place(name, number, position);
  }
}
void Silver::sprayLine(string name, int number, int spawns, Vec3 start, Vec3 end) {
  srand(static_cast < unsigned int > (time(nullptr)));

  int dx = end.x - start.x;
  int dy = end.y - start.y;
  int dz = end.z - start.z;

  for (int i = 0; i < spawns; ++i) {
    double t = static_cast < double > (rand()) / RAND_MAX;

    Vec3 position = {
      static_cast < int > (start.x + t * dx),
      static_cast < int > (start.y + t * dy),
      static_cast < int > (start.z + t * dz)
    };

    place(name, number, position);
  }
}
void Silver::sprayOval(string name, int number, int spawns, Vec3 center, Vec3 scale) {
  srand(static_cast < unsigned int > (time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    double angle = static_cast < double > (rand()) / RAND_MAX * 2 * M_PI;
    double distanceX = (static_cast < double > (rand()) / RAND_MAX) * scale.x;
    double distanceY = (static_cast < double > (rand()) / RAND_MAX) * scale.y;

    Vec3 position = {
      center.x + static_cast < int > (distanceX * cos(angle)),
      center.y + static_cast < int > (distanceY * sin(angle)),
      center.z
    };

    place(name, number, position);
  }
}
void Silver::Drawing::sprayLine(int spawns, Vec3 start, Vec3 end, string c) {
  srand(static_cast < unsigned int > (time(nullptr)));

  int dx = end.x - start.x;
  int dy = end.y - start.y;
  int dz = end.z - start.z;

  for (int i = 0; i < spawns; ++i) {
    double t = static_cast < double > (rand()) / RAND_MAX;

    Vec3 position = {
      static_cast < int > (start.x + t * dx),
      static_cast < int > (start.y + t * dy),
      static_cast < int > (start.z + t * dz)
    };

    draw(position, c);
  }
}

void Silver::spray(string name, int number, int spawns, Vec3 center, int range) {
  srand(static_cast < unsigned int > (time(nullptr)));

  for (int i = 0; i < spawns; ++i) {
    int offsetX = rand() % (2 * range + 1) - range;
    int offsetY = rand() % (2 * range + 1) - range;

    Vec3 position = {
      center.x + offsetX,
      center.y + offsetY,
      center.z
    };

    place(name, number, position);
  }
}

void Silver::Drawing::RectangleHollow(Vec3 topLeft, int width, int height, string c) {
  for (int i = 0; i < width; ++i) {
    draw({
      topLeft.x + i,
      topLeft.y,
      topLeft.z
    }, c);
    draw({
      topLeft.x + i,
      topLeft.y + height - 1,
      topLeft.z
    }, c);
  }

  for (int j = 1; j < height - 1; ++j) {
    draw({
      topLeft.x,
      topLeft.y + j,
      topLeft.z
    }, c);
    draw({
      topLeft.x + width - 1,
      topLeft.y + j,
      topLeft.z
    }, c);
  }
}

void Silver::Drawing::Circle(Vec3 center, int radius, string c) {
  for (int y = center.y - radius; y <= center.y + radius; ++y) {
    for (int x = center.x - radius; x <= center.x + radius; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      if (dx * dx + dy * dy <= radius * radius) {
        draw({
          x,
          y,
          center.z
        }, c);
      }
    }
  }
}

void Silver::Drawing::Line(Vec3 start, Vec3 end, string c) {
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
    draw({
      x1,
      y1,
      start.z
    }, c);
    if (x1 == x2 && y1 == y2) break;

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

void Silver::Drawing::Oval(Vec3 center, Vec3 scale, string c) {
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      if ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y)) {
        draw({
          x,
          y,
          center.z
        }, c);
      }
    }
  }
}
void Silver::Drawing::OvalHollow(Vec3 center, Vec3 scale, string c) {
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      int first = ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y));
      scale.x--;
      scale.y--;
      int second = ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y));
      if (first && !second) {
        draw({
          x,
          y,
          center.z
        }, c);
      }
      scale.x++;
      scale.y++;
    }
  }
}

void Silver::Drawing::CircleHollow(Vec3 center, int radius, string c) {
  for (int y = center.y - radius; y <= center.y + radius; ++y) {
    for (int x = center.x - radius; x <= center.x + radius; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;
      int distanceSquared = dx * dx + dy * dy;

      if (distanceSquared >= (radius - 1) * (radius - 1) && distanceSquared <= radius * radius) {
        draw({
          x,
          y,
          center.z
        }, c);
      }
    }
  }
}
void Silver::Rectangle(string name, int number, Vec3 topLeft, int width, int height) {
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      Vec3 pos = {
        topLeft.x + i,
        topLeft.y + j,
        topLeft.z
      };
      place(name, number, pos);
    }
  }
}

void Silver::RectangleHollow(string name, int number, Vec3 topLeft, int width, int height) {
  for (int i = 0; i < width; ++i) {
    place(name, number, {
      topLeft.x + i,
      topLeft.y,
      topLeft.z
    });
    place(name, number, {
      topLeft.x + i,
      topLeft.y + height - 1,
      topLeft.z
    });
  }

  for (int j = 1; j < height - 1; ++j) {
    place(name, number, {
      topLeft.x,
      topLeft.y + j,
      topLeft.z
    });
    place(name, number, {
      topLeft.x + width - 1,
      topLeft.y + j,
      topLeft.z
    });
  }
}

void Silver::Circle(string name, int number, Vec3 center, int radius) {
  for (int y = center.y - radius; y <= center.y + radius; ++y) {
    for (int x = center.x - radius; x <= center.x + radius; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      if (dx * dx + dy * dy <= radius * radius) {
        place(name, number, {
          x,
          y,
          center.z
        });
      }
    }
  }
}

void Silver::Line(string name, int number, Vec3 start, Vec3 end) {
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
    place(name, number, {
      x1,
      y1,
      start.z
    });
    if (x1 == x2 && y1 == y2) break;

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

void Silver::Oval(string name, int number, Vec3 center, Vec3 scale) {
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      if ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y)) {
        place(name, number, {
          x,
          y,
          center.z
        });
      }
    }
  }
}

void Silver::OvalHollow(string name, int number, Vec3 center, Vec3 scale) {
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      int first = ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y));
      scale.x--;
      scale.y--;
      int second = ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y));
      if (first && !second) {
        place(name, number, {
          x,
          y,
          center.z
        });
      }
      scale.x++;
      scale.y++;
    }
  }
}

void Silver::CircleHollow(string name, int number, Vec3 center, int radius) {
  for (int y = center.y - radius; y <= center.y + radius; ++y) {
    for (int x = center.x - radius; x <= center.x + radius; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;
      int distanceSquared = dx * dx + dy * dy;

      if (distanceSquared >= (radius - 1) * (radius - 1) && distanceSquared <= radius * radius) {
        place(name, number, {
          x,
          y,
          center.z
        });
      }
    }
  }
}

void Silver::Camera::setCam3(Vec3 pos, Vec3 scale) {
  silver.camera.CameraPos = pos;
  silver.camera.CameraScale = scale;
}

void Silver::Camera::setCam2(Vec3 pos, Vec3 scale) {
  silver.camera.CameraPos = {
    pos.x,
    pos.y,
    silver.camera.CameraPos.z
  };
  silver.camera.CameraScale = {
    pos.x,
    pos.y,
    silver.camera.CameraScale.z
  };
}

int getObjectIdAtCoordinates(Vec3 pos) {

  for (const auto & entry: workspace) {
    const auto & object = entry.second;

    if (pos == entry.second.object.position) {
      return entry.first;
    }
  }

  return -1;
}

std::string truncateToCellSize(const std::string & cellContent, int cellSize) {
  std::string result;
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
void Silver::Camera::printCam() {
  if (isFirst) {
    silver.setRawMode();
    silver.clear();
  }

  vector < vector < string >> buffer(
    silver.camera.CameraScale.y, vector < string > (silver.camera.CameraScale.x, null_rep)
  );

  float radians = silver.camera.cameraRotation * (M_PI / 180.0);
  float cosTheta = cos(radians);
  float sinTheta = sin(radians);

  int centerX = silver.camera.CameraScale.x / 2;
  int centerY = silver.camera.CameraScale.y / 2;

  bool sign = false;
  string signMessage, signIcon, objectName;

  for (const auto & entry: workspace) {
    const auto & obj = entry.second;

    int dx = obj.object.position.x - silver.camera.CameraPos.x;
    int dy = obj.object.position.y - silver.camera.CameraPos.y;
    int dz = obj.object.position.z - silver.camera.CameraPos.z;

    int rotatedX = round(cosTheta * dx + sinTheta * dy) + centerX;
    int rotatedY = round(-sinTheta * dx + cosTheta * dy) + centerY;

    if (silver.camera.isFlippedHorizonal == -1) {
      rotatedX = silver.camera.CameraScale.x - rotatedX - 1;
    }
    if (silver.camera.isFlippedVertical == -1) {
      rotatedY = silver.camera.CameraScale.y - rotatedY - 1;
    }

    if (rotatedX >= 0 && rotatedX < silver.camera.CameraScale.x &&
      rotatedY >= 0 && rotatedY < silver.camera.CameraScale.y &&
      dz >= -silver.camera.CameraScale.z / 2 && dz <= silver.camera.CameraScale.z / 2) {

      if (obj.transparency == 0) {
        buffer[rotatedY][rotatedX] = obj.shape;

        if (!obj.Components.signMessage.empty()) {
          signMessage = obj.Components.signMessage;
          sign = true;
          signIcon = obj.shape;
          objectName = obj.object.name;
        }
      }
    }
  }

  int mouseX = silver.mouse.cursorPositionX;
  int mouseY = silver.mouse.cursorPositionY;

  vector < string > leftText, rightText;
  int leftTextLines = 0, rightTextLines = 0;

  if (!silver.camera.leftText.empty()) {
    stringstream ss(silver.camera.leftText);
    string line;
    while (getline(ss, line)) {
      leftText.push_back(line);
      leftTextLines++;
    }
  }

  if (!silver.camera.rightText.empty()) {
    stringstream ss(silver.camera.rightText);
    string line;
    while (getline(ss, line)) {
      rightText.push_back(line);
      rightTextLines++;
    }
  }

  int maxLeftWidth = 0, maxRightWidth = 0;
  for (const auto & line: leftText) {
    maxLeftWidth = std::max(maxLeftWidth, static_cast < int > (line.size()));
  }
  for (const auto & line: rightText) {
    maxRightWidth = std::max(maxRightWidth, static_cast < int > (line.size()));
  }

  int tl = silver.camera.leftAlign == 0 ? silver.camera.CameraScale.y / 2 - leftTextLines / 2 :
    (silver.camera.leftAlign == 1 ? 0 : silver.camera.CameraScale.y - leftTextLines);
  int tr = silver.camera.rightAlign == 0 ? silver.camera.CameraScale.y / 2 - rightTextLines / 2 :
    (silver.camera.rightAlign == 1 ? 0 : silver.camera.CameraScale.y - rightTextLines);

  int offsetX = silver.camera.CameraConsolePos.x;
  int offsetY = silver.camera.CameraConsolePos.y;

  switch (anchor) {
  case 0:

    break;
  case 1:
    offsetX -= silver.camera.CameraScale.x / 2;
    break;
  case 2:
    offsetX -= silver.camera.CameraScale.x;
    break;
  case 3:
    offsetY -= silver.camera.CameraScale.y / 2;
    break;
  case 4:
    offsetX -= silver.camera.CameraScale.x / 2;
    offsetY -= silver.camera.CameraScale.y / 2;
    break;
  case 5:
    offsetX -= silver.camera.CameraScale.x;
    offsetY -= silver.camera.CameraScale.y / 2;
    break;
  case 6:
    offsetY -= silver.camera.CameraScale.y;
    break;
  case 7:
    offsetX -= silver.camera.CameraScale.x / 2;
    offsetY -= silver.camera.CameraScale.y;
    break;
  case 8:
    offsetX -= silver.camera.CameraScale.x;
    offsetY -= silver.camera.CameraScale.y;
    break;
  }

  for (int j = 0; j < silver.camera.CameraScale.y; ++j) {
    string leftLine = (j - tl < leftText.size() && j - tl >= 0) ? leftText[j - tl] : "";
    leftLine = string(maxLeftWidth - leftLine.size(), ' ') + leftLine;

    string rightLine = (j - tr < rightText.size() && j - tr >= 0) ? rightText[j - tr] : "";
    rightLine += string(maxRightWidth - rightLine.size(), ' ');

    string line = leftLine + " ";
    for (int i = 0; i < silver.camera.CameraScale.x; ++i) {
      string cellContent = (!hideMouse && i == mouseX && j == mouseY) ? mouseIcon : buffer[j][i];
      if (gridMode && cellContent.size() > silver.camera.cellSize) {
        cellContent = truncateToCellSize(cellContent, silver.camera.cellSize);
      }
      if (fillMode && cellContent.size() < silver.camera.cellSize) {
        cellContent.append(silver.camera.cellSize - cellContent.size(), ' ');
      }

      line += cellContent;
    }
    line += " " + rightLine;

    silver.gotoxy(offsetX, offsetY + j + (silver.camera.topText.empty() ? 0 : 1));
    cout << line << flush;
  }

  displayText(silver.camera.topText, silver.camera.CameraScale.x, maxLeftWidth, true, offsetX);
  displayText(silver.camera.bottomText, silver.camera.CameraScale.x, maxLeftWidth, false, offsetX);

  isFirst = false;
}

void Silver::Camera::displayText(const string & text, int scaleX, int maxLeftWidth, bool isTop, int offsetX) {
  if (!text.empty()) {
    stringstream ss(text);
    string line;
    cout << "\n";
    while (getline(ss, line)) {
      int padding = (scaleX * silver.camera.cellSize - line.size()) / 2 + offsetX;
      padding = std::max(0, padding + maxLeftWidth);
      cout << string(padding, ' ') << line << endl;
    }
  }
}

void Silver::setConsoleTitle(const std::string & title) {
  std::cout << "\033]0;" << title << "\007";
}

void Silver::Camera::photo() {
  if (isFirst) {
    silver.setRawMode();
    silver.clear();
  }

  vector < vector < string >> buffer(
    silver.camera.CameraScale.y, vector < string > (silver.camera.CameraScale.x, null_rep)
  );

  float radians = silver.camera.cameraRotation * (M_PI / 180.0);
  float cosTheta = cos(radians);
  float sinTheta = sin(radians);

  int centerX = silver.camera.CameraScale.x / 2;
  int centerY = silver.camera.CameraScale.y / 2;

  bool sign = false;
  string signMessage, signIcon, objectName;

  for (const auto & entry: workspace) {
    const auto & obj = entry.second;

    int dx = obj.object.position.x - silver.camera.CameraPos.x;
    int dy = obj.object.position.y - silver.camera.CameraPos.y;
    int dz = obj.object.position.z - silver.camera.CameraPos.z;

    int rotatedX = round(cosTheta * dx + sinTheta * dy) + centerX;
    int rotatedY = round(-sinTheta * dx + cosTheta * dy) + centerY;

    if (silver.camera.isFlippedHorizonal == -1) {
      rotatedX = silver.camera.CameraScale.x - rotatedX - 1;
    }
    if (silver.camera.isFlippedVertical == -1) {
      rotatedY = silver.camera.CameraScale.y - rotatedY - 1;
    }

    if (rotatedX >= 0 && rotatedX < silver.camera.CameraScale.x &&
      rotatedY >= 0 && rotatedY < silver.camera.CameraScale.y &&
      dz >= -silver.camera.CameraScale.z / 2 && dz <= silver.camera.CameraScale.z / 2) {

      if (obj.transparency == 0) {
        buffer[rotatedY][rotatedX] = obj.shape;

        if (!obj.Components.signMessage.empty()) {
          signMessage = obj.Components.signMessage;
          sign = true;
          signIcon = obj.shape;
          objectName = obj.object.name;
        }
      }
    }
  }

  int mouseX = silver.mouse.cursorPositionX;
  int mouseY = silver.mouse.cursorPositionY;

  if (!gridMode) {
    for (int j = 0; j < silver.camera.CameraScale.y; ++j) {
      string line;
      for (int i = 0; i < silver.camera.CameraScale.x; ++i) {

        string cellContent = (!hideMouse && i == mouseX && j == mouseY) ? mouseIcon : buffer[j][i];
        line += cellContent;
      }
      cout << line << endl;
    }
  } else {
    for (int j = 0; j < silver.camera.CameraScale.y; ++j) {
      for (int i = 0; i < silver.camera.CameraScale.x; ++i) {
        string cellContent = (!hideMouse && i == mouseX && j == mouseY) ? mouseIcon : buffer[j][i];

        if (fillMode && cellContent.size() < silver.camera.cellSize) {
          cellContent.append(silver.camera.cellSize - cellContent.size(), ' ');
        }
        if (cellContent.size() > silver.camera.cellSize) {
          cellContent = cellContent.substr(0, silver.camera.cellSize);
        }

        cout << cellContent;
      }
      cout << endl;
    }
  }

  if (sign) {
    cout << "\n\n*************************************\n" <<
      signIcon << string(silver.camera.cellSize, ' ') << objectName << endl <<
      signMessage << endl;
  }

  isFirst = false;
}

void Silver::setWorldBounds(Vec3 world) {
  WorldX = world.x;
  WorldY = world.y;
}

vector < int > Silver::seek(string objectName) {
  vector < int > numbers;
  for (auto entry: workspace) {
    if (entry.second.object.name == objectName) {
      numbers.push_back(entry.second.object.number);
    }
  }
  return numbers;
}
vector < int > Silver::findObjects(string name, variant < vector < int > , int > number) {
  vector < int > numbers;

  for (auto & entry: workspace) {

    if (entry.second.object.name == name) {

      if (holds_alternative < vector < int >> (number)) {
        vector < int > numList = get < vector < int >> (number);

        if (find(numList.begin(), numList.end(), entry.second.object.number) != numList.end()) {
          numbers.push_back(entry.first);
        }
      } else if (holds_alternative < int > (number)) {
        int num = get < int > (number);

        if (entry.second.object.number == num || num == all) {
          numbers.push_back(entry.first);
        }
      }
    }
  }

  return numbers;
}

void Silver::destroy(const string & objectName,
  const variant < int, vector < int >> & number) {
  vector < int > keysToRemove;
  vector < string > globalKeysToRemove;

  if (objectName == pointer) {

    if (holds_alternative < int > (number)) {
      int numValue = get < int > (number);
      for (const auto & entry: workspace) {
        if (entry.first == numValue) {
          keysToRemove.push_back(entry.first);
        }
      }
    } else if (holds_alternative < vector < int >> (number)) {
      vector < int > numValues = get < vector < int >> (number);
      for (const auto & entry: workspace) {
        if (find(numValues.begin(), numValues.end(), entry.first) != numValues.end()) {
          keysToRemove.push_back(entry.first);
        }
      }
    }
  } else {
    if (holds_alternative < int > (number)) {
      int numValue = get < int > (number);
      if (numValue == origin) {

        for (const auto & entry: prefabs) {
          if (entry.second.name == objectName) {
            globalKeysToRemove.push_back(entry.first);
          }
        }
      } else {

        for (const auto & entry: workspace) {
          if (entry.second.object.name == objectName &&
            (entry.second.object.number == all || entry.second.object.number == numValue)) {
            keysToRemove.push_back(entry.first);
          }
        }
      }
    } else if (holds_alternative < vector < int >> (number)) {
      vector < int > numValues = get < vector < int >> (number);

      for (const auto & entry: workspace) {
        if (entry.second.object.name == objectName &&
          (entry.second.object.number == all ||
            find(numValues.begin(), numValues.end(), entry.second.object.number) != numValues.end())) {
          keysToRemove.push_back(entry.first);
        }
      }
    }
  }

  for (const auto & key: keysToRemove) {
    workspace.erase(key);
  }

  for (const auto & key: globalKeysToRemove) {
    prefabs.erase(key);
  }
}

void Silver::kill(const string & objectName,
  const variant < int, vector < int >> & number) {
  vector < int > keysToKill;
  set < int > numberSet;

  if (holds_alternative < int > (number)) {
    numberSet.insert(get < int > (number));
  } else {
    numberSet = set < int > (get < vector < int >> (number).begin(),
      get < vector < int >> (number).end());
  }

  if (objectName == pointer) {
    for (const auto & entry: workspace) {

      if (numberSet.count(entry.first)) {
        keysToKill.push_back(entry.first);
      }
    }
  } else {

    for (const auto & entry: workspace) {
      if (entry.second.object.name == objectName &&
        (numberSet.count(all) || numberSet.count(entry.second.object.number))) {
        keysToKill.push_back(entry.first);
      }
    }
  }

  for (const auto & key: keysToKill) {
    killedSprites.insert({
      key,
      workspace.find(key) -> second
    });
    workspace.erase(key);
  }
}

void Silver::revive(const string & objectName,
  const variant < int, vector < int >> & number) {
  vector < int > keysToRevive;
  set < int > numberSet;

  if (holds_alternative < int > (number)) {
    numberSet.insert(get < int > (number));
  } else {
    numberSet = set < int > (get < vector < int >> (number).begin(), get < vector < int >> (number).end());
  }

  if (objectName == pointer) {
    for (const auto & entry: killedSprites) {
      if (numberSet.count(entry.first)) {
        keysToRevive.push_back(entry.first);
      }
    }
  } else {
    for (const auto & entry: killedSprites) {
      if (entry.second.object.name == objectName &&
        (numberSet.count(all) || numberSet.count(entry.second.object.number))) {
        keysToRevive.push_back(entry.first);
      }
    }
  }

  for (const auto & key: keysToRevive) {
    workspace.insert({
      key,
      killedSprites.find(key) -> second
    });
    killedSprites.erase(key);
  }
}

Vec2 Silver::Camera::getScreenPosition(Vec3 pos) {
  int worldX = pos.x;
  int worldY = pos.y;
  float radians = silver.camera.cameraRotation * (M_PI / 180.0);
  float cosTheta = cos(radians);
  float sinTheta = sin(radians);

  int centerX = silver.camera.CameraScale.x / 2;
  int centerY = silver.camera.CameraScale.y / 2;

  float dx = worldX - silver.camera.CameraPos.x;
  float dy = worldY - silver.camera.CameraPos.y;

  int rotatedX = round(cosTheta * dx + sinTheta * dy);
  int rotatedY = round(-sinTheta * dx + cosTheta * dy);

  if (silver.camera.isFlippedHorizonal == -1) {
    rotatedX = WorldX - rotatedX - 1;
  }
  if (silver.camera.isFlippedVertical == -1) {
    rotatedY = WorldY - rotatedY - 1;
  }

  int screenX = rotatedX + centerX;
  int screenY = rotatedY + centerY;

  return {
    screenX,
    screenY
  };
}

void Silver::clear() {
  system("clear");
}

void Silver::Camera::flipCamera(int X, int Y) {
  if (X == 1 || X == -1) {
    silver.camera.isFlippedHorizonal *= X;
  }
  if (Y == 1 || Y == -1) {
    silver.camera.isFlippedVertical *= Y;
  }
}

void Silver::Camera::SetCameraFlip(int X, int Y) {
  if (X == 1 || X == -1) {
    silver.camera.isFlippedHorizonal = X;
  }
  if (Y == 1 || Y == -1) {
    silver.camera.isFlippedVertical = Y;
  }
}

void Silver::Camera::pivotCamera(int angle) {
  silver.camera.cameraRotation = angle;
}

void Silver::Camera::addPivotCamera(int angle) {
  silver.camera.cameraRotation += angle;
}

void Silver::clean(Vec3 first, Vec3 second) {
  auto it = workspace.begin();
  while (it != workspace.end()) {
    Vec3 key = it -> second.object.position;
    int x = key.x;
    int y = key.y;

    if (x >= std::min(first.x, second.x) && x <= std::max(first.x, second.x) &&
      y >= std::min(first.y, second.y) && y <= std::max(first.y, second.y)) {
      it = workspace.erase(it);
    } else {
      ++it;
    }
  }
}

void Silver::loadChunk(const string & file) {
  ifstream fp(file);

  if (!fp.is_open()) {
    cerr << "Error opening file.\n";
    return;
  }

  string buffer;
  int A, B, C, D, E, F;
  int type = -1;

  while (getline(fp, buffer)) {
    if (buffer.rfind("📷", 0) == 0 && type == -1) {
      if (sscanf(buffer.c_str() + sizeof("📷") - 1, "%d %d %d %d %d %d", & A, & B, & C, & D, & E, & F) == 6) {
        Vec3 position = {
          A,
          B,
          C
        };
        Vec3 scale = {
          D,
          E,
          F
        };

        silver.camera.setCam3(position, scale);
        type = 1;
      }
    } else if (buffer.rfind("🗺️", 0) == 0) {
      char context[BUFFER_SIZE];
      int coord1, coord2, coord3;
      if (sscanf(buffer.c_str() + sizeof("🗺️") - 1, " \"%[^\"]\" %d %d %d", context, & coord1, & coord2, & coord3) == 4) {

        silver.draw.draw(Vec3(coord1, coord2, coord3), context);
      }
    } else if (buffer.rfind("💥", 0) == 0) {
      int x1, y1, x2, y2;
      if (sscanf(buffer.c_str() + sizeof("💥") - 1, " %d %d %d %d", & x1, & y1, & x2, & y2) == 4) {
        clean({
          x1,
          y1,
          0
        }, {
          x2,
          y2,
          0
        });
      }
    }
  }
}

void Silver::Camera::moveCamera(Vec3 V) {
  silver.camera.CameraPos.x += V.x;
  silver.camera.CameraPos.y += V.y;
}

void Silver::Camera::shakeCameraOnce(float intensity) {
  float offsetX = intensity * (rand() % 100 / 100.f - 0.f);
  float offsetY = intensity * (rand() % 100 / 100.f - 0.f);

  silver.camera.CameraPos.x += (int) offsetX;
  silver.camera.CameraPos.y += (int) offsetY;
}

void Silver::Camera::shakeCamera(float intensity, int shakes, float delayBetweenShakes) {
  float originalX = silver.camera.CameraPos.x;
  float originalY = silver.camera.CameraPos.y;

  for (int i = 0; i < shakes; ++i) {
    float offsetX = intensity * (rand() % 100 / 100.f - 0.f);
    float offsetY = intensity * (rand() % 100 / 100.f - 0.f);

    silver.camera.CameraPos.x = originalX + (int) offsetX;
    silver.camera.CameraPos.y = originalY + (int) offsetY;

    silver.wait(delayBetweenShakes);
  }

  silver.camera.CameraPos.x = originalX;
  silver.camera.CameraPos.y = originalY;
}

pair < int, int > Silver::getConsoleSize() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, & w);
  int width = w.ws_col;
  int height = w.ws_row;
  return {
    width,
    height
  };
}

Vec2 Silver::getConsoleCenter() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, & w);
  int width = w.ws_col;
  int height = w.ws_row;
  return Vec2(width - width / 2, height - height / 2);
}

void Silver::Camera::zoomCamera(Vec3 V) {
  silver.camera.CameraScale.x += V.x;
  silver.camera.CameraScale.y += V.y;
  if (silver.camera.CameraScale.x < 0) silver.camera.CameraScale.x = 0;
  if (silver.camera.CameraScale.y < 0) silver.camera.CameraScale.y = 0;
}

atomic < bool > VMouse = false;
mutex mouseMutex;

void VMouseOn(int l, int r, int u, int d, int c) {
  while (VMouse.load()) {
    char det = silver.keyboard.getKey();

    {

      lock_guard < mutex > lock(mouseMutex);
      if (det == l) {
        silver.mouse.cursorPositionX--;
      } else if (det == r) {
        silver.mouse.cursorPositionX++;
      } else if (det == u) {
        silver.mouse.cursorPositionY--;
      } else if (det == d) {
        silver.mouse.cursorPositionY++;
      } else if (det == c) {

      }
    }

  }
}

void Silver::Mouse::startVMouse(int l, int r, int u, int d, int c) {
  silver.camera.hideMouse = false;
  if (VMouse.load()) return;

  VMouse.store(true);
  mouseKey = c;
  thread vmouseThread(VMouseOn, l, r, u, d, c);
  vmouseThread.detach();
}

void Silver::Mouse::stopVMouse() {
  VMouse.store(false);
}

void startLoading() {
  silver.clear();
  printf("--------------------------------\n");
  printf(". . . . . . . . . . . . . . . . \n");
  printf("--------------------------------\n");
  silver.gotoxy(0, 1);
  cout << "#" << flush;
}

void Silver::createObject(const string name,
  const string shape) {
  if (prefabs.count(name) > 0) {
    return;
  }

  objects A;
  A.name = name;
  A.shape = shape;
  A.transparency = 0;
  prefabs[name] = A;
}

int Silver::place(string objectName, int number, Vec3 position) {
  if (number < 0) {
    number = 0;
  }
  if (prefabs.count(objectName) == 0) {
    return -1;
  }

  auto entry = prefabs.find(objectName);
  if (entry != prefabs.end()) {
    mesh X;
    X.object.position = position;
    X.object.number = number;
    X.object.name = objectName;
    X.shape = entry -> second.shape;
    X.transparency = entry -> second.transparency;;
    X.Components = entry -> second.Components;

    sprites_count++;
    workspace.insert({
      sprites_count - 1,
      X
    });
  }
  return sprites_count - 1;
}

int Silver::put(string objectName, Vec3 position) {
  return place(objectName, 0, position);

}

atomic < bool > isRunningCam(false);
thread videoThread;
mutex camMutex;

void Silver::Camera::startVideo(int FPS) {

  lock_guard < mutex > lock(camMutex);
  if (isRunningCam) return;

  isRunningCam = true;
  videoThread = thread([this, FPS] {
    while (isRunningCam) {
      this -> printCam();
      this_thread::sleep_for(chrono::milliseconds(static_cast < int > (1000.f / FPS)));
    }
  });
}

void Silver::hold() {
  wait(2147483647);
}

void Silver::Camera::endVideo() {
  {
    lock_guard < mutex > lock(camMutex);
    if (!isRunningCam) return;

    isRunningCam = false;
  }
  if (videoThread.joinable()) {
    videoThread.join();
  }
}

void Silver::Camera::restartVideo(int FPS) {
  endVideo();
  startVideo(FPS);
}
void Silver::wait(float time) {
  this_thread::sleep_for(chrono::milliseconds(static_cast < int > (time)));
}

vector < int > Silver::spriteAt3(Vec3 pos) {
  vector < int > names;

  for (auto & entry: workspace) {
    if (entry.second.object.position.x == pos.x && entry.second.object.position.y == pos.y &&
      entry.second.object.position.z == pos.z) {

      names.push_back(entry.first);
    }
  }

  return names;
}
vector < int > Silver::spriteAt2(Vec3 pos) {
  vector < int > names;

  for (auto & entry: workspace) {
    if (entry.second.object.position.x == pos.x && entry.second.object.position.y == pos.y) {

      names.push_back(entry.first);
    }
  }

  return names;
}

void Silver::setObjectXY(const variant < int, vector < int >> objectID, Vec3 pos) {
  int x = pos.x;
  int y = pos.y;

  vector < int > idsToUpdate;

  if (holds_alternative < int > (objectID)) {
    int singleID = get < int > (objectID);
    if (singleID == origin) {
      return;
    } else if (singleID == all) {
      for (auto & entry: workspace) {
        idsToUpdate.push_back(entry.first);
      }
    } else {
      idsToUpdate.push_back(singleID);
    }
  } else {
    const vector < int > & ids = get < vector < int >> (objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id: idsToUpdate) {
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh & X = it -> second;
      int z = X.object.position.z;

      X.object.position = {
        x,
        y,
        z
      };
    }
  }
}

void Silver::setObjectRandom(const variant < int, vector < int >> objectID,
  const pair < int, int > & xRange,
    const pair < int, int > & yRange) {
  vector < int > idsToUpdate;

  if (holds_alternative < int > (objectID)) {
    int singleID = get < int > (objectID);
    if (singleID == origin) {
      return;
    } else if (singleID == all) {
      for (auto & entry: workspace) {
        idsToUpdate.push_back(entry.first);
      }
    } else {
      idsToUpdate.push_back(singleID);
    }
  } else {
    const vector < int > & ids = get < vector < int >> (objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id: idsToUpdate) {
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh & X = it -> second;
      int z = X.object.position.z;

      // Determine x and y values based on the pairs provided
      int x = (xRange.first == xRange.second) ? xRange.first : getRandom(xRange.first, xRange.second);
      int y = (yRange.first == yRange.second) ? yRange.first : getRandom(yRange.first, yRange.second);

      // Update position with determined x and y
      X.object.position = {
        x,
        y,
        z
      };
    }
  }
}

void Silver::setObjectPositionToSprite(const variant < int, vector < int >> objectIDs, int spriteID) {
  // Get the target sprite's position
  Vec3 targetPos = silver.GetMeshOf(spriteID).object.position;

  // Determine which IDs to update based on the variant type
  vector < int > idsToUpdate;
  if (holds_alternative < int > (objectIDs)) {
    int singleID = get < int > (objectIDs);
    if (singleID != origin) {
      idsToUpdate.push_back(singleID);
    }
  } else {
    const vector < int > & idList = get < vector < int >> (objectIDs);
    for (int id: idList) {
      if (id != origin) {
        idsToUpdate.push_back(id);
      }
    }
  }

  // Set each object's position to match the target sprite's position
  for (int id: idsToUpdate) {
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh & objMesh = it -> second;
      objMesh.object.position = targetPos; // Set position to match target sprite
    }
  }
}

void Silver::glideObjectPositionToSprite(const variant < int, vector < int >> objectIDs, int spriteID, float speed) {
  // Get the target sprite's position
  Vec3 targetPos = silver.GetMeshOf(spriteID).object.position;

  // Prepare objects to glide to the target position
  int maxSteps = 0;
  vector < tuple < int, int, int >> idsToUpdate;

  // Determine which IDs to update based on the variant type
  if (holds_alternative < int > (objectIDs)) {
    int singleID = get < int > (objectIDs);
    if (singleID != origin) {
      Vec3 currentPos = silver.GetMeshOf(singleID).object.position;
      int x_steps = abs(targetPos.x - currentPos.x);
      int y_steps = abs(targetPos.y - currentPos.y);
      maxSteps = max({
        maxSteps,
        x_steps,
        y_steps
      });

      idsToUpdate.emplace_back(singleID, x_steps, y_steps);
    }
  } else {
    const vector < int > & idList = get < vector < int >> (objectIDs);
    for (int id: idList) {
      if (id == origin) continue;

      Vec3 currentPos = silver.GetMeshOf(id).object.position;
      int x_steps = abs(targetPos.x - currentPos.x);
      int y_steps = abs(targetPos.y - currentPos.y);
      maxSteps = max({
        maxSteps,
        x_steps,
        y_steps
      });

      idsToUpdate.emplace_back(id, x_steps, y_steps);
    }
  }

  // Calculate step duration based on speed
  float stepDuration = speed / maxSteps;

  for (int i = 0; i < maxSteps; ++i) {
    silver.wait(stepDuration);

    for (const auto & [id, x_steps, y_steps]: idsToUpdate) {
      Vec3 currentPos = silver.GetMeshOf(id).object.position;

      if (i < x_steps && (i % (maxSteps / x_steps) == 0)) {
        int dirX = (targetPos.x > currentPos.x) ? 1 : -1;
        silver.moveObjectX(id, dirX);
      }

      if (i < y_steps && (i % (maxSteps / y_steps) == 0)) {
        int dirY = (targetPos.y > currentPos.y) ? 1 : -1;
        silver.moveObjectY(id, dirY);
      }
    }
  }
}

// Utility function to generate a random integer within a range
int Silver::getRandom(int min, int max) {
  static std::mt19937 rng(std::random_device {}());
  std::uniform_int_distribution < int > dist(min, max);
  return dist(rng);
}

void Silver::setObjectPosition(const variant < int, vector < int >> objectID, Vec3 pos) {
  int x = pos.x;
  int y = pos.y;
  int z = pos.z;

  vector < int > idsToUpdate;

  if (holds_alternative < int > (objectID)) {
    int singleID = get < int > (objectID);
    if (singleID == origin) {
      return;
    }
    idsToUpdate.push_back(singleID);
  } else {
    const vector < int > & ids = get < vector < int >> (objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id: idsToUpdate) {
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh & X = it -> second;
      X.object.position = {
        x,
        y,
        z
      };

    }
  }
}

void Silver::moveObjectXY(const variant < int, vector < int >> objectID, Vec3 pos) {
  int dx = pos.x;
  int dy = pos.y;

  vector < int > idsToUpdate;

  if (holds_alternative < int > (objectID)) {
    int singleID = get < int > (objectID);
    if (singleID == origin) {
      return;
    }
    idsToUpdate.push_back(singleID);
  } else {
    const vector < int > & ids = get < vector < int >> (objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id: idsToUpdate) {
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh & X = it -> second;
      X.object.position += Vec3(dx, dy, 0);
      int x = X.object.position.x;
      int y = X.object.position.y;
      int z = X.object.position.z;

    }
  }
}

void Silver::moveObjectPosition(const variant < int, vector < int >> objectID, Vec3 pos) {
  int dx = pos.x;
  int dy = pos.y;
  int dz = pos.z;

  vector < int > idsToUpdate;

  if (holds_alternative < int > (objectID)) {
    int singleID = get < int > (objectID);
    if (singleID == origin) {
      return;
    }
    idsToUpdate.push_back(singleID);
  } else {
    const vector < int > & ids = get < vector < int >> (objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id: idsToUpdate) {
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh & X = it -> second;
      X.object.position += Vec3(dx, dy, dz);
      int x = X.object.position.x;
      int y = X.object.position.y;
      int z = X.object.position.z;

    }
  }
}

void Silver::moveObjectX(const variant < int, vector < int >> objectID, int x_offset) {
  int dx = x_offset;
  vector < int > idsToUpdate;

  if (holds_alternative < int > (objectID)) {
    int singleID = get < int > (objectID);
    if (singleID == origin) {
      return;
    }
    idsToUpdate.push_back(singleID);
  } else {
    const vector < int > & ids = get < vector < int >> (objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id: idsToUpdate) {
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh & X = it -> second;
      X.object.position += Vec3(dx, 0, 0);
      int x = X.object.position.x;
      int y = X.object.position.y;
      int z = X.object.position.z;

    }
  }
}

void Silver::moveObjectY(const variant < int, vector < int >> objectID, int y_offset) {
  int dy = y_offset;
  vector < int > idsToUpdate;

  if (holds_alternative < int > (objectID)) {
    int singleID = get < int > (objectID);
    if (singleID == origin) {
      return;
    }
    idsToUpdate.push_back(singleID);
  } else {
    const vector < int > & ids = get < vector < int >> (objectID);
    idsToUpdate.insert(idsToUpdate.end(), ids.begin(), ids.end());
  }

  for (int id: idsToUpdate) {
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh & X = it -> second;
      X.object.position += Vec3(0, dy, 0);
      int x = X.object.position.x;
      int y = X.object.position.y;
      int z = X.object.position.z;

    }
  }
}

void Silver::setObjectX(const string & name,
  const variant < int, vector < int >> & ids, int x) {
  vector < int > idsToUpdate;

  if (holds_alternative < int > (ids)) {
    int singleId = get < int > (ids);
    if (singleId == origin) {
      return;
    }
    if (singleId == all) {

      for (const auto & entry: workspace) {
        if (name == pointer) {
          if (entry.first) {
            idsToUpdate.push_back(entry.first);
          }
        } else if (entry.second.object.name == name) {
          idsToUpdate.push_back(entry.first);
        }
      }
    } else {

      for (const auto & entry: workspace) {
        if (name == pointer) {
          if (entry.first == singleId) {
            idsToUpdate.push_back(entry.first);
          }
        } else if (entry.second.object.name == name && entry.first == singleId) {
          idsToUpdate.push_back(entry.first);
        }
      }
    }
  } else {
    const vector < int > & idList = get < vector < int >> (ids);
    for (const auto & entry: workspace) {
      if (name == pointer) {
        if (find(idList.begin(), idList.end(), entry.first) != idList.end()) {
          idsToUpdate.push_back(entry.first);
        }
      } else if (entry.second.object.name == name &&
        find(idList.begin(), idList.end(), entry.first) != idList.end()) {
        idsToUpdate.push_back(entry.first);
      }
    }
  }

  for (const auto & id: idsToUpdate) {
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh X = it -> second;
      int y = X.object.position.y;
      int z = X.object.position.z;
      X.object.position = Vec3(x, y, z);

    }
  }
}
void Silver::glideObjectX(const variant < int, vector < int >> & ids, int x_offset, float speed, ...) {
  va_list args;
  va_start(args, speed);
  bool setPosition = va_arg(args, int);
  va_end(args);

  int maxSteps = 0;
  vector < pair < int, int >> idsToUpdate;

  if (holds_alternative < int > (ids)) {
    int singleId = get < int > (ids);
    if (singleId == origin) return;

    Vec3 current_pos = silver.GetMeshOf(singleId).object.position;
    int targetX = setPosition ? x_offset : current_pos.x + x_offset;
    int steps = abs(targetX - current_pos.x);
    maxSteps = max(maxSteps, steps);

    idsToUpdate.emplace_back(singleId, steps);
  } else {
    const vector < int > & idList = get < vector < int >> (ids);
    for (const auto & id: idList) {
      if (id == origin) continue;

      Vec3 current_pos = silver.GetMeshOf(id).object.position;
      int targetX = setPosition ? x_offset : current_pos.x + x_offset;
      int steps = abs(targetX - current_pos.x);
      maxSteps = max(maxSteps, steps);

      idsToUpdate.emplace_back(id, steps);
    }
  }

  float stepDuration = speed / maxSteps;

  for (int i = 0; i < maxSteps; ++i) {
    silver.wait(stepDuration);

    for (const auto & [id, steps]: idsToUpdate) {
      if (i < steps && (i % (maxSteps / steps) == 0)) {
        int dir = (x_offset > 0) ? 1 : -1;
        silver.moveObjectX(id, dir);
      }
    }
  }
}

void Silver::glideObjectY(const variant < int, vector < int >> & ids, int y_offset, float speed, ...) {
  va_list args;
  va_start(args, speed);
  bool setPosition = va_arg(args, int);
  va_end(args);

  int maxSteps = 0;
  vector < pair < int, int >> idsToUpdate;

  if (holds_alternative < int > (ids)) {
    int singleId = get < int > (ids);
    if (singleId == origin) return;

    Vec3 current_pos = silver.GetMeshOf(singleId).object.position;
    int targetY = setPosition ? y_offset : current_pos.y + y_offset;
    int steps = abs(targetY - current_pos.y);
    maxSteps = max(maxSteps, steps);

    idsToUpdate.emplace_back(singleId, steps);
  } else {
    const vector < int > & idList = get < vector < int >> (ids);
    for (const auto & id: idList) {
      if (id == origin) continue;

      Vec3 current_pos = silver.GetMeshOf(id).object.position;
      int targetY = setPosition ? y_offset : current_pos.y + y_offset;
      int steps = abs(targetY - current_pos.y);
      maxSteps = max(maxSteps, steps);

      idsToUpdate.emplace_back(id, steps);
    }
  }

  float stepDuration = speed / maxSteps;

  for (int i = 0; i < maxSteps; ++i) {
    silver.wait(stepDuration);

    for (const auto & [id, steps]: idsToUpdate) {
      if (i < steps && (i % (maxSteps / steps) == 0)) {
        int dir = (y_offset > 0) ? 1 : -1;
        silver.moveObjectY(id, dir);
      }
    }
  }
}
int animationID = 0;

void Silver::Animation::stopAnimation(variant < int, vector < int >> objID, bool reset) {
  vector < int > ids;
  if (holds_alternative < int > (objID)) {
    ids.push_back(get < int > (objID));
  } else {
    ids = get < vector < int >> (objID);
  }

  for (int id: ids) {
    workspace[id].playingID = -1;
    if (reset) {
      workspace[id].shape = workspace[id].animationBuffer;
    }
  }
}

void Silver::Animation::applyAnimation(variant < int, vector < int >> objID,
  const std::string & filename) {
  std::ifstream file(filename);
  if (!file) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return;
  }

  std::vector < std::string > animationFrames;
  float fps = -1;
  int transition = -1;

  std::string line;
  while (std::getline(file, line)) {
    if (line.compare(0, 6, "$write") == 0) {
      std::string frames = line.substr(7);
      std::stringstream ss(frames);
      std::string frame;
      while (std::getline(ss, frame, ',')) {
        animationFrames.push_back(frame);
      }
    } else if (line.compare(0, 4, "$for") == 0) {
      std::string command = line.substr(5);
      std::stringstream ss(command);
      int repeatCount;
      std::string frame;
      if (ss >> repeatCount >> frame) {
        for (int i = 0; i < repeatCount; ++i) {
          animationFrames.push_back(frame);
        }
      }
    } else if (line.compare(0, 4, "FPS ") == 0) {
      std::string fpsStr = line.substr(4);
      fps = std::stof(fpsStr);
    } else if (line.compare(0, 12, "TRANSITION ") == 0) {
      std::string transitionStr = line.substr(12);
      if (transitionStr == "IMMIDIATE") {
        transition = -1;
      } else {
        transition = std::stoi(transitionStr);
      }
    }
  }

  applyAnimationRaw(objID, animationFrames, fps, transition);
}

void Silver::Animation::applyAnimationRaw(variant < int, vector < int >> objID, vector < string > animation, float fps, int transition) {
  vector < int > ids;
  if (holds_alternative < int > (objID)) {
    ids.push_back(get < int > (objID));
  } else {
    ids = get < vector < int >> (objID);
  }

  int newAnimationID = animationID++;

  auto animationThread = [this, ids, animation, fps, newAnimationID, transition]() mutable {
    int frameIndex = 0;
    int totalFrames = animation.size();

    for (int id: ids) {
      stopAnimation(id, false);
      workspace[id].playingID = newAnimationID;
      workspace[id].animationBuffer = workspace[id].shape;
    }

    while (!ids.empty()) {
      string currentFrame = animation[frameIndex];

      for (auto it = ids.begin(); it != ids.end();) {
        int id = * it;

        if (workspace[id].playingID == newAnimationID) {

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

      silver.wait(static_cast < int > (1000 / fps));

      frameIndex = (frameIndex + 1) % totalFrames;
    }

    for (int id: ids) {
      workspace[id].playingID = -1;
      workspace[id].shape = workspace[id].animationBuffer;
    }
  };

  std::thread(animationThread).detach();
}

void Silver::glideObjectRandom(const variant < int, vector < int >> & ids,
  const pair < int, int > & xRange,
    const pair < int, int > & yRange, float speed) {
  int maxSteps = 0;
  vector < tuple < int, int, int, Vec3 >> idsToUpdate;

  auto getRandom = [](int min, int max) {
    static std::mt19937 rng(std::random_device {}());
    std::uniform_int_distribution < int > dist(min, max);
    return dist(rng);
  };

  if (holds_alternative < int > (ids)) {
    int singleId = get < int > (ids);
    if (singleId == origin) return;

    Vec3 current_pos = silver.GetMeshOf(singleId).object.position;
    int targetX = getRandom(xRange.first, xRange.second);
    int targetY = getRandom(yRange.first, yRange.second);

    int x_steps = abs(targetX - current_pos.x);
    int y_steps = abs(targetY - current_pos.y);
    maxSteps = max({
      maxSteps,
      x_steps,
      y_steps
    });

    idsToUpdate.emplace_back(singleId, x_steps, y_steps, Vec3 {
      targetX,
      targetY,
      current_pos.z
    });
  } else {
    const vector < int > & idList = get < vector < int >> (ids);
    for (const auto & id: idList) {
      if (id == origin) continue;

      Vec3 current_pos = silver.GetMeshOf(id).object.position;
      int targetX = getRandom(xRange.first, xRange.second);
      int targetY = getRandom(yRange.first, yRange.second);

      int x_steps = abs(targetX - current_pos.x);
      int y_steps = abs(targetY - current_pos.y);
      maxSteps = max({
        maxSteps,
        x_steps,
        y_steps
      });

      idsToUpdate.emplace_back(id, x_steps, y_steps, Vec3 {
        targetX,
        targetY,
        current_pos.z
      });
    }
  }

  float stepDuration = speed / maxSteps;

  for (int i = 0; i < maxSteps; ++i) {
    silver.wait(stepDuration);

    for (const auto & [id, x_steps, y_steps, target_pos]: idsToUpdate) {
      Vec3 current_pos = silver.GetMeshOf(id).object.position;

      if (i < x_steps && (i % (maxSteps / x_steps) == 0)) {
        int dirX = (target_pos.x > current_pos.x) ? 1 : -1;
        silver.moveObjectX(id, dirX);
      }

      if (i < y_steps && (i % (maxSteps / y_steps) == 0)) {
        int dirY = (target_pos.y > current_pos.y) ? 1 : -1;
        silver.moveObjectY(id, dirY);
      }
    }
  }
}
void Silver::setObjectY(const string & name,
  const variant < int, vector < int >> & ids, int y) {
  vector < int > idsToUpdate;

  if (holds_alternative < int > (ids)) {
    int singleId = get < int > (ids);
    if (singleId == origin) return;

    if (singleId == all) {
      for (const auto & entry: workspace) {
        if ((name == pointer && entry.first) || entry.second.object.name == name) {
          idsToUpdate.push_back(entry.first);
        }
      }
    } else {
      for (const auto & entry: workspace) {
        if ((name == pointer && entry.first == singleId) ||
          (entry.second.object.name == name && entry.first == singleId)) {
          idsToUpdate.push_back(entry.first);
        }
      }
    }
  } else {
    const vector < int > & idList = get < vector < int >> (ids);
    for (const auto & entry: workspace) {
      if ((name == pointer && find(idList.begin(), idList.end(), entry.first) != idList.end()) ||
        (entry.second.object.name == name && find(idList.begin(), idList.end(), entry.first) != idList.end())) {
        idsToUpdate.push_back(entry.first);
      }
    }
  }

  for (const auto & id: idsToUpdate) {
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      mesh & currentMesh = it -> second;
      int x = currentMesh.object.position.x;
      int z = currentMesh.object.position.z;

      currentMesh.object.position.y = y;

      auto value = currentMesh;

    }
  }
}

struct termios orig_termios;

void Silver::setRawMode() {
  struct termios raw;

  tcgetattr(STDIN_FILENO, & orig_termios);

  raw = orig_termios;
  raw.c_lflag &= ~(ICANON | ECHO);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, & raw);
}

void Silver::resetMode() {

  tcsetattr(STDIN_FILENO, TCSAFLUSH, & orig_termios);
}

void Silver::Threading::tRun(function < void() > func) {
  thread t(func);
  t.detach();
}

void Silver::Threading::dRun(function < void() > func, float time) {
  thread t([func, time]() {
    this_thread::sleep_for(chrono::milliseconds(static_cast < int > (time)));
    func();
  });
  t.detach();
}

void Silver::Threading::createThread(const string & name,
  function < void() > func) {
  lock_guard < mutex > lock(thread_mutex);

  if (threads.find(name) != threads.end()) return;

  auto threadInfo = make_shared < ThreadInfo > ();

  threadInfo -> t = thread([this, name, func, threadInfo]() {
    unique_lock < mutex > lock(threadInfo -> mtx);
    while (!threadInfo -> stop) {

      if (threadInfo -> paused) {
        threadInfo -> cv.wait(lock);
      } else {
        func();
        break;
      }
    }
  });

  threads[name] = threadInfo;
}

void Silver::Threading::destroyThread(const string & name) {
  lock_guard < mutex > lock(thread_mutex);
  if (threads.find(name) != threads.end()) {
    {
      lock_guard < mutex > th_lock(threads[name] -> mtx);
      threads[name] -> stop = true;
      threads[name] -> cv.notify_all();

    }
    if (threads[name] -> t.joinable()) threads[name] -> t.join();
    threads.erase(name);
  }
}

void Silver::Threading::stopAllThreads() {
  lock_guard < mutex > lock(thread_mutex);
  for (auto & pair: threads) {
    {
      lock_guard < mutex > th_lock(pair.second -> mtx);
      pair.second -> stop = true;
      pair.second -> cv.notify_all();

    }
    if (pair.second -> t.joinable()) pair.second -> t.join();
  }
  threads.clear();
}

void Silver::Threading::pauseThread(const string & name) {
  lock_guard < mutex > lock(thread_mutex);
  if (threads.find(name) != threads.end() && !threads[name] -> paused) {
    lock_guard < mutex > th_lock(threads[name] -> mtx);
    threads[name] -> paused = true;

  }
}

void Silver::Threading::resumeThread(const string & name) {
  lock_guard < mutex > lock(thread_mutex);
  if (threads.find(name) != threads.end() && threads[name] -> paused) {

    lock_guard < mutex > th_lock(threads[name] -> mtx);
    threads[name] -> paused = false;
    threads[name] -> cv.notify_all();

  }
}

vector < Vec3 > Silver::getOvalPoints(Vec3 center, Vec3 scale) {
  vector < Vec3 > points;
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;
      if ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y)) {
        points.push_back({
          x,
          y,
          center.z
        });
      }
    }
  }
  return points;
}

vector < Vec3 > Silver::getOvalHollowPoints(Vec3 center, Vec3 scale) {
  vector < Vec3 > points;
  for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
    for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
      int dx = x - center.x;
      int dy = y - center.y;

      bool outer = ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y));
      scale.x--;
      scale.y--;
      bool inner = ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y));
      scale.x++;
      scale.y++;

      if (outer && !inner) {
        points.push_back({
          x,
          y,
          center.z
        });
      }
    }
  }
  return points;
}

vector < Vec3 > Silver::getLinePoints(Vec3 start, Vec3 end) {
  vector < Vec3 > points;

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
    points.push_back({
      x1,
      y1,
      start.z
    });
    if (x1 == x2 && y1 == y2) break;

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

vector < Vec3 > Silver::getRectanglePoints(Vec3 topLeft, int width, int height) {
  vector < Vec3 > points;
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      points.push_back({
        topLeft.x + i,
        topLeft.y + j,
        topLeft.z
      });
    }
  }
  return points;
}
vector < Vec3 > Silver::getRectangleHollowPoints(Vec3 topLeft, int width, int height) {
  vector < Vec3 > points;

  for (int i = 0; i < width; ++i) {
    points.push_back({
      topLeft.x + i,
      topLeft.y,
      topLeft.z
    });
    points.push_back({
      topLeft.x + i,
      topLeft.y + height - 1,
      topLeft.z
    });
  }

  for (int j = 1; j < height - 1; ++j) {
    points.push_back({
      topLeft.x,
      topLeft.y + j,
      topLeft.z
    });
    points.push_back({
      topLeft.x + width - 1,
      topLeft.y + j,
      topLeft.z
    });
  }

  return points;
}

Vec3 Silver::getLocation(int id) {
  auto it = workspace.find(id);
  if (it != workspace.end()) {
    return workspace[id].object.position;
  }
  return {
    -1,
    -1,
    -1
  };
}

bool isAlive(int obj) {
  return workspace.find(obj) != workspace.end();
}

void Silver::Threading::applyFunction(const vector < int > & ids,
  function < void(int) > func, char mode, ...) {
  vector < int > targetObjects;
  int key = 0;
  Vec3 pos = {
    0,
    0,
    0
  };
  va_list args;
  va_start(args, mode);

  if (mode == 'k') {
    key = va_arg(args, int);
  } else if (mode == 'm') {
    key = silver.mouse.mouseKey;

  }
  va_end(args);

  for (int obj: ids) {
    lock_guard < mutex > lock(thread_mutex);

    if (mode == 'h') pos = va_arg(args, Vec3);
    if (mode == 'H') pos = silver.camera.getScreenPosition(silver.getLocation(obj));
    if (mode == 'c') pos = va_arg(args, Vec3);
    if (mode == 'C') pos = silver.camera.getScreenPosition(silver.getLocation(obj));
    activeThreads.emplace_back([obj, func, mode, key, pos]() {
      while (isAlive(obj)) {
        if (mode == 'k' || mode == 'm' || mode == 'c' || mode == 'C') {
          while (true) {
            silver.keyboard.getKey();
            if (silver.keyboard.isKey(key)) {
              if (mode == 'c' || mode == 'C') {
                Vec3 cursorPosition = {
                  silver.mouse.cursorPositionX,
                  silver.mouse.cursorPositionY,
                  0
                };
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
            Vec3 cursorPosition = {
              silver.mouse.cursorPositionX,
              silver.mouse.cursorPositionY,
              0
            };
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

void Silver::applyTag(const vector < int > & ids,
  const string & tag) {
  for (int id: ids) {
    auto it = workspace.find(id);
    if (it != workspace.end()) {
      it -> second.object.tags.push_back(tag);
    }
  }
}

vector < int > Silver::seekTag(const string & tag) {
  vector < int > result;

  for (const auto & entry: workspace) {
    if (find(entry.second.object.tags.begin(), entry.second.object.tags.end(), tag) != entry.second.object.tags.end()) {
      result.emplace_back(entry.first);
    }
  }

  return result;
}

vector < string > Silver::getTag(int id) {
  auto it = workspace.find(id);
  if (it != workspace.end()) {
    return it -> second.object.tags;
  }

  return {};
}

void threadedFlow(int id) {

  if (workspace.find(id) == workspace.end()) return;

  int maxGenerations = workspace[id].Components.maximumDistance;
  std::queue < std::pair < int, float >> flowQueue;
  flowQueue.push({
    id,
    0
  });

  const double waitTimeReductionFactor = 0.9;
  int remainingDistance = maxGenerations;
  int lastGen = -1;

  while (!flowQueue.empty() && remainingDistance > 0 && workspace[id].Components.isFluid) {
    auto[currentId, generationCount] = flowQueue.front();
    flowQueue.pop();

    double waitTime = workspace[currentId].Components.diffusionSpeed * std::pow(waitTimeReductionFactor, generationCount);

    if (static_cast < int > (generationCount) != lastGen) {
      silver.wait(waitTime);
      lastGen = generationCount;
      remainingDistance--;
      if (remainingDistance == 0) break;
    }

    bool flowed = false;

    for (int dir = 0; dir < 4; ++dir) {
      int newX = workspace[currentId].object.position.x;
      int newY = workspace[currentId].object.position.y;

      switch (dir) {
      case 0:
        newY++;
        break; // Up
      case 1:
        newY--;
        break; // Down
      case 2:
        newX--;
        break; // Left
      case 3:
        newX++;
        break; // Right
      }

      bool canFlow = true;

      {

        for (const auto & [neighborId, neighborMesh]: workspace) {
          if (neighborMesh.object.position.x == newX && neighborMesh.object.position.y == newY) {
            canFlow = !neighborMesh.Components.preventFlowing && !neighborMesh.Components.isFluid;
            break;
          }
        }
      }

      if (canFlow) {

        int newDupId = silver.duplicate(currentId)[0];
        workspace[newDupId].object.position.x = newX;
        workspace[newDupId].object.position.y = newY;
        flowQueue.push({
          newDupId,
          generationCount + 1
        });
        flowed = true;
      }
    }

  }
}

void flow(int id) {
  std::thread(threadedFlow, id).detach(); // Start flow in a detached thread
}

void Silver::Fluid::Liquify(const variant < int, vector < int >> & IDs, double diffusionSpeed, int maximumDistance) {
  auto liquifyMesh = [ & ](int id) {
    if (workspace.find(id) != workspace.end()) {
      mesh & m = workspace[id];
      m.Components.isFluid = true;
      m.Components.diffusionSpeed = diffusionSpeed;
      m.Components.maximumDistance = maximumDistance;
      flow(id); // Start flowing this mesh
    }
  };

  // Process the IDs variant
  if (std::holds_alternative < int > (IDs)) {
    int id = std::get < int > (IDs);
    liquifyMesh(id); // Apply to a single ID
  } else if (std::holds_alternative < std::vector < int >> (IDs)) {
    for (int id: std::get < std::vector < int >> (IDs)) {
      liquifyMesh(id); // Apply to each ID in the vector
    }
  }

}
vector < int > Silver::duplicate(const variant < int, vector < int >> & IDs) {
  vector < int > duplicatedIDs; // Store the IDs of duplicated sprites

  // Check if `IDs` holds a single int
  if (std::holds_alternative < int > (IDs)) {
    int id = std::get < int > (IDs);

    // Duplicate the sprite and store the new ID
    sprites_count++;
    workspace[sprites_count - 1] = workspace[id];
    duplicatedIDs.push_back(sprites_count - 1); // Add the new ID to the result list

  } else if (std::holds_alternative < vector < int >> (IDs)) {
    // Duplicate each ID in the vector
    for (int id: std::get < vector < int >> (IDs)) {
      sprites_count++;
      workspace[sprites_count - 1] = workspace[id];
      duplicatedIDs.push_back(sprites_count - 1); // Add each new ID to the result list
    }
  }

  return duplicatedIDs; // Return all duplicated IDs
}

void Silver::Fluid::Unliquify(const variant < int, vector < int >> & IDs) {
  auto unliquifyMesh = [ & ](int id) {
    if (workspace.find(id) != workspace.end()) {
      mesh & m = workspace[id];
      m.Components.isFluid = false;
    }
  };

  // Process the IDs variant
  if (std::holds_alternative < int > (IDs)) {
    int id = std::get < int > (IDs);
    unliquifyMesh(id); // Apply to a single ID
  } else if (std::holds_alternative < std::vector < int >> (IDs)) {
    for (int id: std::get < std::vector < int >> (IDs)) {
      unliquifyMesh(id); // Apply to each ID in the vector
    }
  }
}
void Silver::Fluid::Unsolidify(const variant < int, vector < int >> & IDs) {
  auto unsolidifyMesh = [ & ](int id) {
    if (workspace.find(id) != workspace.end()) {
      mesh & m = workspace[id];
      m.Components.preventFlowing = false;
    }
  };

  // Process the IDs variant
  if (std::holds_alternative < int > (IDs)) {
    int id = std::get < int > (IDs);
    unsolidifyMesh(id); // Apply to a single ID
  } else if (std::holds_alternative < std::vector < int >> (IDs)) {
    for (int id: std::get < std::vector < int >> (IDs)) {
      unsolidifyMesh(id); // Apply to each ID in the vector
    }
  }
}
void Silver::Fluid::Solidify(const variant < int, vector < int >> & IDs) {
  auto solidifyMesh = [ & ](int id) {
    if (workspace.find(id) != workspace.end()) {
      mesh & m = workspace[id];
      m.Components.preventFlowing = true;
    }
  };

  // Process the IDs variant
  if (std::holds_alternative < int > (IDs)) {
    int id = std::get < int > (IDs);
    solidifyMesh(id); // Apply to a single ID
  } else if (std::holds_alternative < std::vector < int >> (IDs)) {
    for (int id: std::get < std::vector < int >> (IDs)) {
      solidifyMesh(id); // Apply to each ID in the vector
    }
  }
}

zone Silver::buildZone(variant < tuple < int, int, int, int > , vector < tuple < int, int, int, int >>> shape) {
  return zone(shape);
}

bool Silver::isInZone(const zone & z, Vec3 pos) {
  int pointX = pos.x, pointY = pos.y;
  if (holds_alternative < tuple < int, int, int, int >> (z.shape)) {
    const auto & rect = get < tuple < int,
      int, int, int >> (z.shape);
    int x = get < 0 > (rect);
    int y = get < 1 > (rect);
    int width = get < 2 > (rect);
    int height = get < 3 > (rect);

    return (pointX >= x && pointX <= x + width) &&
      (pointY >= y && pointY <= y + height);
  } else {
    const auto & rects = get < vector < tuple < int,
      int, int, int >>> (z.shape);
    for (const auto & rect: rects) {
      int x = get < 0 > (rect);
      int y = get < 1 > (rect);
      int width = get < 2 > (rect);
      int height = get < 3 > (rect);

      if ((pointX >= x && pointX <= x + width) &&
        (pointY >= y && pointY <= y + height)) {
        return true;
      }
    }
  }
  return false;
}

void Silver::Timer::resetTimer(string name) {
  running[name] = false;
  duration[name] = 0;
}

long long Silver::Timer::getDuration(string name) const {

  if (running.find(name) != running.end() && running.at(name) == true) {
    auto currentTime = chrono::high_resolution_clock::now();
    return duration.at(name) + chrono::duration_cast < chrono::milliseconds > (currentTime - startTime.at(name)).count();
  }

  if (duration.find(name) != duration.end()) {
    return duration.at(name);
  }

  return 0;
}

void Silver::Timer::startTimer(string name) {

  if (running.find(name) == running.end() || running.at(name) == false) {
    startTime[name] = chrono::high_resolution_clock::now();
    running[name] = true;
  }
}

void Silver::Timer::stopTimer(string name) {
  if (running[name]) {
    auto endTime = chrono::high_resolution_clock::now();
    duration[name] += chrono::duration_cast < chrono::milliseconds > (endTime - startTime.at(name)).count();
    running[name] = false;
  }
}

float Silver::Timer::getDeltaTime(string name) const {
  return deltaTime.at(name);
}

void Silver::Timer::update(string name) {
  auto currentTime = chrono::high_resolution_clock::now();

  deltaTime[name] = chrono::duration < float > (currentTime - lastTime.at(name)).count();

  lastTime[name] = currentTime;
}
