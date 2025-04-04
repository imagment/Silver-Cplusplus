#include <algorithm>
#include <atomic>
#include <cmath>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <map>
#include <mutex>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "Silver.hpp"


  
  // Public methods
  void AddCamera(Camera *camera);
  void StartVideoProcessing();
  void StopVideoProcessing();

  // Accessors for active cameras
  const std::vector<Camera *> GetActiveCameras();
  void SetActiveCameras(const std::vector<Camera *> &cameras);


  // Member variables
  std::vector<Camera *> activeCameras;

  std::vector<std::vector<float>> hierarchyBuffer;
  std::vector<std::vector<std::string>> renderBuffer;

  int consoleWidth;
  int consoleHeight;



double FPS = 10;

using namespace std;


string TruncateTochunkSize(const string cellContent, int chunkSize) {
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

    if (currentWidth + 1 > chunkSize) {
      break;
    }

    result += cellContent.substr(i, charBytes);
    currentWidth += 1;

    i += charBytes;
  }

  return result;
}






const std::vector<Camera*>
GetActiveCameras() {
  return activeCameras;
}

int previousConsoleWidth = 0;
int previousConsoleHeight = 0;

void Camera::RenderFrame() {
  auto consoleSize = GetConsoleSize();
  int consoleWidth = consoleSize.x;
  int consoleHeight = consoleSize.y;
  
  Vector3 cameraScale = scale;
  Vector3 cameraDisplayPosition = displayPosition;
  
  if(useRelativeTransform) {
    cameraScale.x = consoleWidth * cameraRect.width;
    cameraScale.y = consoleHeight * cameraRect.height;
    cameraDisplayPosition.x = (consoleWidth + 1) * cameraRect.x;
    cameraDisplayPosition.y = (consoleHeight + 1) * cameraRect.y;
  }

  vector<std::string> leftTextLines, rightTextLines;
  int leftTextLinesCount = 0, rightTextLinesCount = 0;

  if (!Camera::rightText.empty()) {
    stringstream ss(Camera::rightText);
    std::string line;
    while (getline(ss, line)) {
      rightTextLines.push_back(line);
      rightTextLinesCount++;
    }
  }
  if (!Camera::leftText.empty()) {
    stringstream ss(Camera::leftText);
    std::string line;
    while (getline(ss, line)) {
      leftTextLines.push_back(line);
      leftTextLinesCount++;
    }
  }
  int topTextLinesCount = 0;
  int bottomTextLinesCount = 0;

  vector<std::string> topTextLines;
  vector<std::string> bottomTextLines;
  
  if (!Camera::topText.empty()) {
    stringstream ss(Camera::topText);
    std::string line;
    while (getline(ss, line)) {
      topTextLines.push_back(line);
      ++topTextLinesCount;
    }
  }

  if (!Camera::bottomText.empty()) {
    stringstream ss(Camera::bottomText);
    std::string line;
    while (getline(ss, line)) {
      bottomTextLines.push_back(line);
      ++bottomTextLinesCount;
    }
  }
  int maxLeftWidth = 0, maxRightWidth = 0;
  for (const auto &line : leftTextLines) {
    maxLeftWidth = std::max(maxLeftWidth, static_cast<int>(line.size()));
  }
  for (const auto &line : rightTextLines) {
    maxRightWidth = std::max(maxRightWidth, static_cast<int>(line.size()));
  }
  
  if (consoleWidth > maxLeftWidth + maxRightWidth + cameraScale.x) cameraScale -= maxLeftWidth + maxRightWidth;
  if (consoleHeight > topTextLinesCount + bottomTextLinesCount + cameraScale.y) cameraScale -= topTextLinesCount + bottomTextLinesCount;
    
  
  if (cameraScale.x == 0 || cameraScale.y == 0 ||
      cameraScale.z == 0)
    return;
    
  // Detect console scale changes
  if (consoleWidth != previousConsoleWidth ||
      consoleHeight != previousConsoleHeight) {
    Clear(); // Clear console to handle size changes
    previousConsoleWidth = consoleWidth;
    previousConsoleHeight = consoleHeight;
  }
  float hierarchy = this->hierarchy; // Default value if hierarchy is not found

  // Fetch hierarchy from active cameras in CameraManager
 

  // Prepare the renderBuffer for rendering
  std::vector<std::vector<std::string>> renderBuffer(cameraScale.y, std::vector<std::string>(cameraScale.x, "\0"));

  for (int row = 0; row < renderBuffer.size(); row++) {
    for (int str = 0; str < renderBuffer[row].size(); str++) {
      if (row % static_cast<int>(patternOccurrenceRate.y) == 0 &&
          str % static_cast<int>(patternOccurrenceRate.x) == 0 &&
          renderBuffer[row][str] == "\0") {
        
        // Fill from renderBuffer[row][str] to the length of backgroundPattern
        size_t patternLength = backgroundPattern.size();
        for (size_t i = 0; i < patternLength; ++i) {
          if (str + i < renderBuffer[row].size()) {
            renderBuffer[row][str + i] = backgroundPattern[i];
          }
        }
        // Skip to the end of the pattern length
        str += patternLength - 1;
      } else {
        renderBuffer[row][str] = " ";
      }
    }
  }
  
  if (showOutOfStagePatterns) {
    for (int row = 0; row < renderBuffer.size(); row++) {
      for (int str = 0; str < renderBuffer[row].size(); str++) {
        if (row % static_cast<int>(patternOccurrenceRate.y) == 0 &&
          str % static_cast<int>(patternOccurrenceRate.x) == 0 &&
          renderBuffer[row][str] == "\0") {
        
          // Fill from renderBuffer[row][str] to the length of backgroundPattern
          size_t patternLength = outOfStagePattern.size();
          for (size_t i = 0; i < patternLength; ++i) {
            if (str + i < renderBuffer[row].size()) {
              renderBuffer[row][str + i] = outOfStagePattern[i];
            }
          }
          // Skip to the end of the pattern length
          str += patternLength - 1;
        } else {
          renderBuffer[row][str] = " ";
        }
      }
    }
  }

  int worldRangeMinX = StageArea.x;
  int worldRangeMaxX = StageArea.x + StageArea.width - 1;
  int worldRangeMinY = StageArea.y;
  int worldRangeMaxY = StageArea.y + StageArea.height - 1;

  float radians = rotation * (M_PI / 180.0);
  float cosTheta = cos(radians);
  float sinTheta = sin(radians);

  vector<std::shared_ptr<Actor>> Viewable;
  double angle = rotation; // Camera's rotation angle in radians
  double cosAngle = std::cos(angle);
  double sinAngle = std::sin(angle);

  auto rotatePointAroundCenter = [&cosAngle, &sinAngle](Vector2 point,
                                                        Vector2 center) {
    // Translate point to origin, rotate, then translate back
    double translatedX = point.x - center.x;
    double translatedY = point.y - center.y;

    double rotatedX = cosAngle * translatedX - sinAngle * translatedY;
    double rotatedY = sinAngle * translatedX + cosAngle * translatedY;

    return Vector2((int)(rotatedX + center.x), (int)(rotatedY + center.y));
  };
  
  for (const auto entry : Workspace) {
    auto obj = entry.second;
    Transform* objTransform = obj->GetComponent<Transform>();
    SpriteRenderer* objSpriteRenderer = obj->GetComponent<SpriteRenderer>();
    
    if(objTransform == nullptr || objSpriteRenderer == nullptr) continue;

    Vector2 size = objSpriteRenderer->GetSize();
    #ifdef DEVELOPPER_DEBUG_MODE
      std::cout << "Actor Size: " << size.x << " " << size.y << std::endl;
    #endif
    Vector3 location = objTransform->position;

    Vector3 scale = objTransform->scale;

    location.x = round(location.x);
    location.y = round(location.y);
    location.z = round(location.z);
    
    if (location.z - scale.z >  position.z + cameraScale.z / 2 ||
        location.z + scale.z < position.z + cameraScale.z / 2 - cameraScale.z ||
        cameraScale.z == 0)
      continue;
      
    if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f) continue;
      
    // Check if the object is part of UI or SpriteRenderer
    if (obj->GetComponent<UI>() != nullptr) {
        location.x = round(obj->GetComponent<Transform>()->position.x + position.x - abs(cameraScale.x) / 2);
        location.y = round(obj->GetComponent<Transform>()->position.y + position.y - abs(cameraScale.y) / 2);
    }
    

    auto spriteRenderer = obj->GetComponent<SpriteRenderer>();
    auto transform = obj->GetComponent<Transform>();
 
    Vector2 pivot = obj->GetComponent<SpriteRenderer>()->pivot;

    if (obj->GetComponent<SpriteRenderer>()->isTransparent) {
      continue; // Skip transparent objects
    }

    Vector2 r1, r2;

    std::tuple<int, int, int, int> seek = obj->GetComponent<SpriteRenderer>()->GetPivotBounds();
 

    Vector2 topLeft =
        Vector2(location.x - get<0>(seek), location.y - get<2>(seek));
    Vector2 topRight =
        Vector2(location.x + get<1>(seek), location.y - get<2>(seek));
    Vector2 bottomLeft =
        Vector2(location.x - get<0>(seek), location.y + get<3>(seek));
    Vector2 bottomRight =
        Vector2(location.x + get<1>(seek), location.y + get<3>(seek));

    // Apply the rotation to all four corners
    Vector2 cameraCenter = position;

    topLeft = rotatePointAroundCenter(topLeft, cameraCenter);
    topRight = rotatePointAroundCenter(topRight, cameraCenter);
    bottomLeft = rotatePointAroundCenter(bottomLeft, cameraCenter);
    bottomRight = rotatePointAroundCenter(bottomRight, cameraCenter);

    // Update r1 and r2 based on the rotated corners
    r1 = Vector2((double)std::min({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x}),
            (double) std::min({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y}));

    r2 = Vector2((double)std::max({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x}),
              (double)   std::max({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y}));

    #ifdef DEVELOPPER_DEBUG_MODE
      std::cout << "Actor Name: " << obj->name << std::flush;
      getchar();
    #endif
    // Apply camera scaling to the bounding box after rotation and flipping

    // Check if the object is within the camera bounds
    if (r2.x < position.x - abs(cameraScale.x - cameraScale.x / 2) ||
        r1.x > position.x + abs(cameraScale.x / 2) ||
        r2.y < position.y - abs(cameraScale.y - cameraScale.y / 2) ||
        r1.y > position.y + abs(cameraScale.y / 2)) {
      continue; // Skip object if it's out of bounds
    }

    Viewable.push_back(obj);
  }

  // Sort objects based on their z position (for layering)
  int flip = 1;
  if (cameraScale.z < 0)
    flip = -1;
  std::stable_sort(Viewable.begin(), Viewable.end(),
    [&](const std::shared_ptr<Actor> a, const std::shared_ptr<Actor> b) {
        bool aIsUI = a->GetComponent<UI>() != nullptr;
        bool bIsUI = b->GetComponent<UI>() != nullptr;

        if (aIsUI != bIsUI)
            return !aIsUI;

        return a->GetComponent<Transform>()->position.z -
                   abs(a->GetComponent<Transform>()->scale.z) / 2 * flip >
               b->GetComponent<Transform>()->position.z -
                   abs(b->GetComponent<Transform>()->scale.z) / 2 * flip;
    });

  
  for (const auto entry : Viewable) {
    Vector3 pos = entry->GetComponent<Transform>()->position;
    double rot = entry->GetComponent<Transform>()->rotation;
    Vector3 scl = entry->GetComponent<Transform>()->scale;
    Vector2 pivot = entry->GetComponent<SpriteRenderer>()->GetPivot();

    // Calculate the object's size after rotation and scaling
    Vector2 objectSize = entry->GetComponent<SpriteRenderer>()->GetSize();

    // Calculate the bounding box of the object after rotation
    Vector2 r1, r2;
    std::tuple<int, int, int, int> seek = entry->GetComponent<SpriteRenderer>()->GetPivotBounds();
    #ifdef DEVELOPPER_DEBUG_MODE
      printf("Pivot bounds: %d %d %d %d\n",get<0>(seek), get<1>(seek), get<2>(seek),get<3>(seek));
    #endif
     
    Vector2 topLeft = Vector2(pos.x - get<0>(seek), pos.y - get<2>(seek));
    Vector2 topRight = Vector2(pos.x + get<1>(seek), pos.y - get<2>(seek));
    Vector2 bottomLeft = Vector2(pos.x - get<0>(seek), pos.y + get<3>(seek));
    Vector2 bottomRight = Vector2(pos.x + get<1>(seek), pos.y + get<3>(seek));

    // Apply rotation to the bounding box corners
    topLeft = rotatePointAroundCenter(topLeft, position );
    topRight = rotatePointAroundCenter(topRight, position );
    bottomLeft = rotatePointAroundCenter(bottomLeft, position );
    bottomRight = rotatePointAroundCenter(bottomRight, position );

    // Update r1 and r2 based on the rotated corners
    r1 = {std::min({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x}),
          std::min({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y})};
    r2 = {std::max({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x}),
          std::max({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y})};
    #ifdef DEVELOPPER_DEBUG_MODE
      printf("Object Rect: %lf %lf %lf %lf\n",r1.x,r1.y,r2.x,r2.y);
      getchar();
    #endif
    SpriteRenderer *sprite = entry->GetComponent<SpriteRenderer>();
    // Render the object
    for (int i = r1.x; i <= r2.x; i++) {
      for (int j = r1.y; j <= r2.y; j++) {
        Vector2 pivot = sprite->GetPivot();
        //printf("[%d %d]", i,j);
        std::string cellStr = sprite->GetCellString(i - pos.x + pivot.x, j - pos.y + pivot.y);
        
        // Calculate the screen position
        int x = cameraScale.x - cameraScale.x / 2 + (i - position.x);
        int y = cameraScale.y - cameraScale.y / 2 + (j - position.y);

        // Update the renderBuffer if the position is within bounds
        if (y >= 0 && y < cameraScale.y && x >= 0 && x < cameraScale.x) {
          std::string stripped = StripAnsi(cellStr);
          if (stripped != " " && stripped!="\0")
            renderBuffer[y][x] = cellStr;
            
        }
        
      }
     
    }
  }

  #ifdef DEVELOPPER_DEBUG_MODE
      getchar();
  #endif
  int mouseX = cursorPositionX;
  int mouseY = cursorPositionY;


  

  int tl, tr;

  tl = (cameraScale.y - leftTextLinesCount) * leftAlign;

  tr = (cameraScale.y - rightTextLinesCount) * rightAlign;
  
  Vector2 anchor = anchor.Clamp(Vector2(0,0), Vector2(1,1));
  // Initialize offsets
  int offsetX = cameraDisplayPosition.x + (consoleWidth - cameraScale.x/2) * anchor.x;
  int offsetY = cameraDisplayPosition.y + (consoleHeight - cameraScale.y/2) * anchor.y;
  

  for (const auto &line : leftTextLines) {
    maxLeftWidth = std::max(maxLeftWidth, static_cast<int>(line.size()));
  }

  


  for (const auto &line : leftTextLines) {
    maxLeftWidth = std::max(maxLeftWidth, static_cast<int>(line.size()));
  }

  int topTextOffsetX = offsetX;

  topTextOffsetX += maxLeftWidth;

  

  // Precompute frequently used values
  int maxConsoleWidth = consoleWidth;
  int maxConsoleHeight = consoleHeight;
  int cameraWidth = cameraScale.x;

  int renderedHeight = cameraScale.y;
  for (int i = 0; i < topTextLines.size(); ++i) {
      int lineOffsetX = offsetX + maxLeftWidth;
      lineOffsetX += (cameraScale.x - topTextLines[i].size()) * topAlign;
      int currentY = offsetY + i;

      if (currentY >= 0 && currentY < consoleHeight) {
          if (lineOffsetX < consoleWidth) {
              int maxWidth = consoleWidth - lineOffsetX; // Max characters that fit in the line
              string slicedText = topTextLines[i].substr(0, maxWidth);
              Gotoxy(lineOffsetX, currentY);
              cout << slicedText << flush;
          }
      }
  }


  if (!sideLimit) {

    renderedHeight = std::max(
        renderedHeight, std::max(leftTextLinesCount, rightTextLinesCount));
  }

  for (int j = 0; j < renderedHeight; ++j) {
    string leftLine =
      (j - tl < leftTextLines.size() && j - tl >= 0)
        ? leftTextLines[j - tl]
        : "";
    leftLine = string(maxLeftWidth - leftLine.size(), ' ') + leftLine;

    string rightLine =
      (j - tr < rightTextLines.size() && j - tr >= 0)
        ? rightTextLines[j - tr]
        : "";
    rightLine += string(maxRightWidth - rightLine.size(), ' ');

    string line = leftLine;
    int availableWidth = consoleWidth - offsetX - maxRightWidth;

    if (j < cameraScale.y) {
      for (int i = 0; i < cameraScale.x; ++i) {
        string cellContent = (!(hideMouse || hideMouse) && i == mouseX && j == mouseY) ? mouseIcon : renderBuffer[j][i];

        if (i > availableWidth) break;
        line += cellContent;
      }
    } else {
     line += string(max(0, min((int)cameraScale.x, availableWidth - (int)line.size())), ' ');

    }
    line += rightLine;


    int currentY = offsetY + j + topTextLinesCount;
    if (currentY >= 0 && currentY < consoleHeight) {
      Gotoxy(offsetX, currentY);
      cout << line << flush;
    }
  }



  // Render bottomText
  for (int i = 0; i < bottomTextLines.size(); ++i) {
    int lineOffsetX = offsetX + maxLeftWidth;


     lineOffsetX += (cameraScale.x - bottomTextLines[i].size()) * bottomAlign;
    // Calculate the vertical position for bottom text
    int currentY = offsetY + cameraScale.y + topTextLinesCount + i;

    // Skip lines completely out of console bounds
    if (currentY < 0 || currentY >= consoleHeight)
      continue;

    // Truncate line content to fit console width
    std::string truncatedLine = bottomTextLines[i];
    if (lineOffsetX + truncatedLine.size() > consoleWidth) {
      truncatedLine = truncatedLine.substr(0, consoleWidth - lineOffsetX);
    }

    // Skip rendering if completely out of bounds
    if (lineOffsetX >= consoleWidth || lineOffsetX + truncatedLine.size() <= 0)
      continue;

    // Move cursor and print the line
    Gotoxy(std::max(0, lineOffsetX), currentY);
    std::cout << truncatedLine << std::flush;
  }
}

void Camera::ShakeCameraOnce(float intensity) {
  float offsetX =
      intensity * ((rand() % 100 / 100) * (rand() % 2 == 0 ? 1 : -1));
  float offsetY =
      intensity * ((rand() % 100 / 100) * (rand() % 2 == 0 ? 1 : -1));

  position.x += static_cast<int>(offsetX);
  position.y += static_cast<int>(offsetY);
}

void Camera::ShakeCamera(float intensity, int shakes,
                         float delayBetweenShakes) {
  float originalX = position.x;
  float originalY = position.y;

  for (int i = 0; i < shakes; ++i) {
    float offsetX = intensity * (rand() % 100 / 100 - 0);
    float offsetY = intensity * (rand() % 100 / 100 - 0);

    position.x = originalX + (int)offsetX;
    position.y = originalY + (int)offsetY;

    Wait(delayBetweenShakes);
  }

  position.x = originalX;
  position.y = originalY;
}

// Camera method implementations
void Camera::StartVideo() {

  if (!isRunningCam) {
    isRunningCam = true;

    // Register the camera with the CameraManager, passing the hierarchy
    AddCamera(this);
  }
}

void Camera::StopVideo() {
  auto consoleSize = GetConsoleSize();
  int consoleWidth = consoleSize.x;
  int consoleHeight = consoleSize.y;
  
  Vector3 cameraScale = scale;
  Vector3 cameraDisplayPosition = displayPosition;
  
  if(useRelativeTransform) {
    cameraScale.x = consoleWidth * cameraRect.width;
    cameraScale.y = consoleHeight * cameraRect.height;
    cameraDisplayPosition.x = consoleWidth * cameraRect.x;
    cameraDisplayPosition.y = consoleHeight * cameraRect.y;
  }

  
  
  std::lock_guard<std::mutex> lock(camMutex);
  if (isRunningCam) {
    isRunningCam = false;

    
    // Find the camera in the activeCameras list and get the associated
    // CameraRegion
    auto it = std::find_if(
        activeCameras.begin(), activeCameras.end(),
        [this](const Camera* camera) {
          return camera == this; // Compare the Camera pointer
        });

    if (it != activeCameras.end()) {
      // Extract the CameraRegion associated with this camera
      // Clean the camera region on the console
      for (int y = cameraDisplayPosition.y - cameraScale.y / 2; y < cameraDisplayPosition.y + cameraScale.y - cameraScale.y / 2; y++) {
        Gotoxy(cameraDisplayPosition.x - cameraScale.x / 2, y);
        for (int x = cameraDisplayPosition.x - cameraScale.x / 2; x < cameraDisplayPosition.x + cameraScale.x - cameraScale.x / 2; x++) {
          std::cout << " " << std::flush; // Output blank spaces to clean up the area
        }
      }

      // Remove the camera from the activeCameras list in CameraManager
      activeCameras.erase(it);
    }
  }
}

void AddCamera(Camera *camera) {

  // Check if the camera is already in the activeCameras vector
  auto it =
      std::find_if(activeCameras.begin(), activeCameras.end(),
                   [camera](const Camera* cam) {
                     return cam == camera;
                   });

  // If the camera is not already in the activeCameras vector, add it
  if (it == activeCameras.end()) {
    activeCameras.push_back(camera);
  }

  // Sort cameras by hierarchy in descending order (higher hierarchy first)
  std::sort(activeCameras.begin(), activeCameras.end(),
            [](const Camera* a,
               const Camera* b) {
              return a->hierarchy > b->hierarchy;
            });

  // Start the video processing if it's not already running
  StartVideoProcessing();
}


std::mutex CameraCoutMutex; // For synchronized console output
bool isRunning = false;
std::thread videoThread; // To hold the video thread

void CleanupAndExit() {
  
  {
    // Stop all cameras and clear the active cameras
    for (auto camera : activeCameras) {
      if (camera) {
        camera->StopVideo();
      }
    }
    activeCameras.clear();
    Clear();
  }

  // If the video thread is running, join it before exiting
  if (videoThread.joinable()) {
    videoThread.join(); // Wait for the video thread to finish
  }

  std::exit(0); // Ensure normal program exit
}

// Signal handler for SIGINT
void SignalHandler(int signum) { CleanupAndExit(); }
void StartVideoProcessing() {
  if (isRunning) {
    return; // If already running, do nothing
  }

  // Register signal handler for SIGINT
  atexit(CleanupAndExit);
  std::signal(SIGINT, SignalHandler);
  isRunning = true;

  // Launch the video processing thread
  videoThread = std::thread([]() {
    auto lastFrameTime = std::chrono::steady_clock::now();
    
    while (isRunning) {
      auto now = std::chrono::steady_clock::now();
      std::chrono::duration<float> elapsed = now - lastFrameTime;
      
      if (elapsed.count() >= 1.0f / FPS) {
        // Process cameras
        {
          if (activeCameras.empty()) {
            break; // Stop the loop if there are no active cameras
          }

          for (auto camera : activeCameras) {
            if (camera && camera->isRunningCam) {
              camera->RenderFrame();
            }
          }
        }
        
        lastFrameTime = now; // Update the last frame time
      }
      
      std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000 / FPS))); // Allow thread to yield to other processes

    }
  });

  // Detach the thread (run independently)
  if (videoThread.joinable()) {
    videoThread.detach();
  }
}


Vector3 Camera::getScale() {
  return scale;
}

Rect Camera::getCameraZone() {
  auto consoleSize = GetConsoleSize();
  int consoleWidth = consoleSize.x;
  int consoleHeight = consoleSize.y;
  
  Vector3 cameraScale = scale;
  Vector3 cameraDisplayPosition = displayPosition;
  
  if(useRelativeTransform) {
    cameraScale.x = consoleWidth * cameraRect.width;
    cameraScale.y = consoleHeight * cameraRect.height;
    cameraDisplayPosition.x = (consoleWidth + 1) * cameraRect.x;
    cameraDisplayPosition.y = (consoleHeight + 1) * cameraRect.y;
  }


  vector<std::string> leftTextLines, rightTextLines;
  int leftTextLinesCount = 0, rightTextLinesCount = 0;

  if (!Camera::rightText.empty()) {
    stringstream ss(Camera::rightText);
    std::string line;
    while (getline(ss, line)) {
      rightTextLines.push_back(line);
      rightTextLinesCount++;
    }
  }
  if (!Camera::leftText.empty()) {
    stringstream ss(Camera::leftText);
    std::string line;
    while (getline(ss, line)) {
      leftTextLines.push_back(line);
      leftTextLinesCount++;
    }
  }
  int topTextLinesCount = 0;
  int bottomTextLinesCount = 0;

  vector<std::string> topTextLines;
  vector<std::string> bottomTextLines;
  
  if (!Camera::topText.empty()) {
    stringstream ss(Camera::topText);
    std::string line;
    while (getline(ss, line)) {
      topTextLines.push_back(line);
      ++topTextLinesCount;
    }
  }

  if (!Camera::bottomText.empty()) {
    stringstream ss(Camera::bottomText);
    std::string line;
    while (getline(ss, line)) {
      bottomTextLines.push_back(line);
      ++bottomTextLinesCount;
    }
  }
  int maxLeftWidth = 0, maxRightWidth = 0;
  for (const auto &line : leftTextLines) {
    maxLeftWidth = std::max(maxLeftWidth, static_cast<int>(line.size()));
  }
  for (const auto &line : rightTextLines) {
    maxRightWidth = std::max(maxRightWidth, static_cast<int>(line.size()));
  }
  
  if (consoleWidth > maxLeftWidth + maxRightWidth + cameraScale.x) cameraScale -= maxLeftWidth + maxRightWidth;
  if (consoleHeight > topTextLinesCount + bottomTextLinesCount + cameraScale.y) cameraScale -= topTextLinesCount + bottomTextLinesCount;
  
 
  return Rect(cameraDisplayPosition.x, cameraDisplayPosition.y, cameraScale.x, cameraScale.y);
}

void Camera::EraseCamera() {
  Rect cameraRegion = getCameraZone();

  for (int j = 0; j < cameraRegion.height; j++) {
    if (Gotoxy(cameraRegion.x, j + cameraRegion.y)) {
      std::string clearLine(cameraRegion.width, ' ');
      std::cout << clearLine << std::flush;
    } 
  }
}

void Camera::setScale(Vector3 scale) {
  EraseCamera();
  this->scale=scale;
}
