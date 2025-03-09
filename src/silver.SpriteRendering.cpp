#include "silver.hpp"

Vector2 SpriteRenderer::GetSize() {
  auto transform = (parent->GetComponent<Transform>());

  double rotation = transform->rotation;
  Vector3 scale = transform->scale;

  int height = 0, width = 0;
  std::stringstream ss(shape);
  std::string line;
  while (std::getline(ss, line, '\n')) {
    height++;
    if (line.size() > width)
      width = line.size();
  }

  float radians = rotation * M_PI / 180.0f;

  // Define the 4 corners of the rectangle
  Vector2 corners[4] = {Vector2(0, 0), Vector2(0, height - 1),
                        Vector2(width - 1, 0), Vector2(width - 1, height - 1)};

  float minX = std::numeric_limits<float>::max();
  float maxX = std::numeric_limits<float>::lowest();
  float minY = std::numeric_limits<float>::max();
  float maxY = std::numeric_limits<float>::lowest();

  // Rotate each corner around the pivot and find the new bounding box
  for (int i = 0; i < 4; i++) {
    // Translate corner to origin
    float dx = corners[i].x - pivot.x;
    float dy = corners[i].y - pivot.y;

    // Apply rotation
    float newX = dx * cos(radians) - dy * sin(radians) + pivot.x;
    float newY = dx * sin(radians) + dy * cos(radians) + pivot.y;

    // Update bounding box
    minX = std::min(minX, newX);
    maxX = std::max(maxX, newX);
    minY = std::min(minY, newY);
    maxY = std::max(maxY, newY);
  }

  // Calculate the new width and height from the bounding box
  float newWidth = maxX - minX + 1;
  float newHeight = maxY - minY + 1;

  return Vector2((int)(newWidth * scale.x), (int)(newHeight * scale.y));
}

Vector2 SpriteRenderer::RotatePoint(int column, int line) {
  auto transform = (parent->GetComponent<Transform>());

  double rotation = transform->rotation;
  // Calculate local coordinates relative to the pivot
  int localX = column - pivot.x;
  int localY = line - pivot.y;

  // Apply clockwise rotation (negative angle for clockwise rotation)
  float radians = rotation * (M_PI / 180.0f); // Negative for clockwise
  int rotatedX = round(localX * cos(radians) - localY * sin(radians));
  int rotatedY = round(localX * sin(radians) + localY * cos(radians));

  // Re-adjust for the pivot's fixed position after transformation
  rotatedX += pivot.x;
  rotatedY += pivot.y;

  return Vector2(rotatedX, rotatedY);
}
std::string SpriteRenderer::GetCellString(int column, int line) {
  auto transform = (parent->GetComponent<Transform>());

  double rotation = transform->rotation;
  Vector3 scale = transform->scale;
  // Calculate local coordinates relative to the pivot
  int localX = column - pivot.x;
  int localY = line - pivot.y;

  // Apply clockwise rotation (negative angle for clockwise rotation)
  float radians = -rotation * (M_PI / 180.0f); // Negative for clockwise
  int rotatedX = round(localX * cos(radians) - localY * sin(radians));
  int rotatedY = round(localX * sin(radians) + localY * cos(radians));

  // Re-adjust for the pivot's fixed position after transformation
  rotatedX += pivot.x;
  rotatedY += pivot.y;

  // Retrieve the shape's string representation
  std::stringstream ss(shape);
  std::string currentLine;
  int currentLineIndex = 0;

  // Iterate through the lines of the shape to find the correct character at the
  // rotated position
  while (std::getline(ss, currentLine, '\n')) {
    if (currentLineIndex == rotatedY) {
      if (rotatedX >= 0 && rotatedX < currentLine.size()) {
        return std::string(1, currentLine[rotatedX]);
      }
      return " "; // Return space if out of bounds
    }
    currentLineIndex++;
  }

  return " "; // Return space if the line is not found
}
std::tuple<int, int, int, int> SpriteRenderer::GetPivotBounds() {
  auto transform = (parent->GetComponent<Transform>());

  double rotation = transform->rotation;
  Vector3 scale = transform->scale;
  int left = 0, right = 0, up = 0, down = 0;

  // Get the size of the shape (assuming size is based on untransformed scale)
  int height = 0, width = 0;
  std::stringstream ss(shape);
  std::string line;
  while (std::getline(ss, line, '\n')) {
    height++;
    if (line.size() > width)
      width = line.size();
  }
  Vector2 size = Vector2(width, height);
  // printf("Size: %d %d\n",size.x, size.y);
  //  Get the transformed coordinates of the four corners
  Vector2 leftUp = RotatePoint(0, 0);
  Vector2 leftDown = RotatePoint(0, size.y - 1);
  Vector2 rightDown = RotatePoint(size.x - 1, size.y - 1);
  Vector2 rightUp = RotatePoint(size.x - 1, 0);

  // Compute the leftmost and rightmost X coordinates
  left = std::min({leftUp.x, leftDown.x, rightDown.x, rightUp.x});
  right = std::max({leftUp.x, leftDown.x, rightDown.x, rightUp.x});

  // Compute the uppermost and lowermost Y coordinates (as per pivot's vertical
  // bounds)
  up = std::min({leftUp.y, leftDown.y, rightDown.y, rightUp.y});
  down = std::max({leftUp.y, leftDown.y, rightDown.y, rightUp.y});

  int l = (pivot.x - left) * scale.x;
  int r = (right - pivot.x) * scale.x;
  int u = (pivot.y - up) * scale.y;
  int d = (down - pivot.y) * scale.y;

  int fl = l + (scale.x - 1) * l / (l + r);
  int fr = r + (scale.x - 1) * r / (l + r);
  int fu = u + (scale.y - 1) * u / (u + d);
  int fd = d + (scale.y - 1) * d / (u + d);

  return std::make_tuple(fl, fr, fu, fd);
}
