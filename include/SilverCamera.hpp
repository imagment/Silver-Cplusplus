#include "Silver.hpp"


class Camera : public Component {
public:
  Camera() = default;
  explicit Camera(Actor *m) : Component(m) {}
  float hierarchy = 0;

  void updateLoop();
  
  static Camera *currentCamera;

  std::string backgroundPattern = ".";
  std::string outOfStagePattern = "~";
  Vector2 patternOccurrenceRate = Vector2(2, 1);
  bool showOutOfStagePatterns = false;
  bool printSpaces = true;

  std::string topText = "";
  std::string rightText = "";
  std::string leftText = "";
  std::string bottomText = "";
  
  bool sideLimit = false;
  bool topDownLimit = true;

  double topAlign = 0.5, bottomAlign =  0.5;
  double leftAlign = 0.5, rightAlign = 0.5;
  
  bool cutOutOfBounds = false;
  bool useRelativeTransform = true;

  Vector3 position;
  double rotation;
  
  
  std::mutex bufferMutex;

  bool hideMouse = true;
  std::map<std::tuple<int, int>, std::string> lastFrame;
  std::atomic<bool> isRunningCam{false};
 
  std::mutex camMutex;
  
  void RenderFrame();
  void StartVideo();
  void StopVideo();
  Vector2 GetScreenPosition(Vector3 pos);
  void ShakeCameraOnce(float intensity);
  void ShakeCamera(float intensity, int shakes, float delayBetweenShakes);
  void EraseCamera();
  
  Rect getCameraZone();
  Vector3 getScale();
  void setScale(Vector3 scale);
  
  void Update(float deltaTime) {
    
  }

private:
  Vector2 displayPosition = Vector2(0, 0);
  Vector2 anchor = Vector2(0, 0);
  Rect cameraRect = Rect(0, 0, 1, 1);
  Vector3 scale = Vector3(20, 20, 20);
};

