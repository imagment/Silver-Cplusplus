#include "Silver.hpp"
#include <vector>
#include <string>
#include <stack>
#include <iostream>


// Function to strip ANSI escape codes
std::string StripAnsi(const std::string& input) {
    std::string result;
    bool insideAnsi = false;

    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\033') { // Start of ANSI sequence
            insideAnsi = true;
        } else if (insideAnsi && input[i] == 'm') { // End of ANSI sequence
            insideAnsi = false;
        } else if (!insideAnsi) {
            result += input[i]; // Add non-ANSI characters to result
        }
    }

    return result;
}

std::vector<std::vector<std::string>> ExtractAnsi(const std::string& input) {
    std::vector<std::vector<std::string>> ansiMatrix;
    std::vector<std::string> currentLine;
    std::string activeAnsi; // Stores active ANSI sequences

    size_t i = 0;
    while (i < input.size()) {
        if (input[i] == '\033') { // Start of an ANSI escape sequence
            size_t end = input.find('m', i);
            if (end != std::string::npos) {
                std::string sequence = input.substr(i, end - i + 1);
                if (sequence == "\033[0m") {
                    activeAnsi.clear(); // Reset ANSI codes
                } else {
                    activeAnsi += sequence; // Append to active ANSI state
                }
                i = end; // Move past the escape sequence
            }
        } else if (input[i] == '\n') {
            ansiMatrix.push_back(currentLine); // Store the current line
            currentLine.clear(); // Start a new line
        } else {
            currentLine.push_back(activeAnsi); // Assign the active ANSI sequence to this character
        }
        ++i;
    }

    // Push the last line if it exists
    if (!currentLine.empty()) {
        ansiMatrix.push_back(currentLine);
    }

    return ansiMatrix;
}



Vector2 SpriteRenderer::GetPivot() {
    Vector2 pivot = this->pivot;
    if(useRelativePivot) {
      pivot = Vector2(static_cast<int>(std::round(this->pivotFactor.x * (spriteWidth - 1))), 
        static_cast<int>(std::round(this->pivotFactor.y * (spriteHeight - 1))));
    }
    
    
    return pivot;
}

Vector2 SpriteRenderer::GetSize() {
    auto transform = parent->GetComponent<Transform>();
    double rotation = transform->rotation;
    Vector3 scale = transform->scale;

    // Get the untransformed size of the cleaned shape
    int height = 0, width = 0;
    
    std::string line;
    while (std::getline(ss, line, '\n')) {
        height++;
        width = std::max(width, static_cast<int>(line.size()));
    }

    // Edge case: Empty shape
    if (width == 0 || height == 0) {
        return Vector2(0, 0);
    }

    Vector2 pivot = this->GetPivot();
    if (useRelativePivot) {
        pivot = Vector2(static_cast<int>(std::round(this->pivotFactor.x * width)),
                        static_cast<int>(std::round(this->pivotFactor.y * height)));
    }

    // Apply scaling before rotation
    double scaledWidth = width * scale.x;
    double scaledHeight = height * scale.y;

    double radians = rotation * M_PI / 180.0f;
    double cosTheta = std::cos(radians);
    double sinTheta = std::sin(radians);

    // Define the 4 corners after scaling (relative to pivot)
    Vector2 corners[4] = {
        Vector2(-pivot.x, -pivot.y),
        Vector2(-pivot.x, scaledHeight - pivot.y - 1),
        Vector2(scaledWidth - pivot.x - 1, -pivot.y),
        Vector2(scaledWidth - pivot.x - 1, scaledHeight - pivot.y - 1)
    };

    // Rotate each corner around the pivot
    double minX = 1e30f, maxX = -1e30f;
    double minY = 1e30f, maxY = -1e30f;

    for (const auto& corner : corners) {
        double rotatedX = corner.x * cosTheta - corner.y * sinTheta;
        double rotatedY = corner.x * sinTheta + corner.y * cosTheta;
     
        minX = std::min(minX, rotatedX);
        maxX = std::max(maxX, rotatedX);
        minY = std::min(minY, rotatedY);
        maxY = std::max(maxY, rotatedY);
    }

    // Compute final bounding box size
    return Vector2(maxX - minX + 1, maxY - minY + 1);
}



Vector2 SpriteRenderer::RotatePoint(int column, int line) {
  Vector2 pivot = this->GetPivot();
  int height = 0, width = 0;
  
  std::string l;
  while (std::getline(ss, l, '\n')) {
      height++;
      width = std::max(width, static_cast<int>(l.size()));
  }
  if(useRelativePivot) pivot = Vector2(static_cast<int>(std::round(this->pivotFactor.x * width)), static_cast<int>(std::round(this->pivotFactor.y * height)));
    
  auto transform = (parent->GetComponent<Transform>());

  double rotation = transform->rotation;
  // Calculate local coordinates relative to the pivot
  int localX = column - pivot.x;
  int localY = line - pivot.y;

  // Apply clockwise rotation (negative angle for clockwise rotation)
  double radians = rotation * (M_PI / 180.0f); // Negative for clockwise
  int rotatedX = round(localX * cos(radians) - localY * sin(radians));
  int rotatedY = round(localX * sin(radians) + localY * cos(radians));

  // Re-adjust for the pivot's fixed position after transformation
  rotatedX += pivot.x;
  rotatedY += pivot.y;

  return Vector2(rotatedX, rotatedY);
}

std::tuple<int, int, int, int> SpriteRenderer::CalculatePivotExpansion() {
    Vector2 pivot = this->GetPivot();
    std::string shape = this->shape;

    auto transform = parent->GetComponent<Transform>();
    Vector3 scale = transform->scale;

    // Determine shape dimensions
    

    if (useRelativePivot) {
        pivot = Vector2(std::round(this->pivotFactor.x * spriteWidth), std::round(this->pivotFactor.y * spriteHeight));
    }

    Vector2 size(spriteWidth, spriteHeight);

    // Get rotated corner positions
    Vector2 leftUp = RotatePoint(0, 0);
    Vector2 leftDown = RotatePoint(0, size.y - 1);
    Vector2 rightDown = RotatePoint(size.x - 1, size.y - 1);
    Vector2 rightUp = RotatePoint(size.x - 1, 0);

    // Compute bounding box
    double lb = std::min({leftUp.x, leftDown.x, rightDown.x, rightUp.x});
    double rb = std::max({leftUp.x, leftDown.x, rightDown.x, rightUp.x});
    double ub = std::min({leftUp.y, leftDown.y, rightDown.y, rightUp.y});
    double db = std::max({leftUp.y, leftDown.y, rightDown.y, rightUp.y});

    // Scale bounds
    double l = (pivot.x - lb) * scale.x;
    double r = (rb - pivot.x) * scale.x;
    double u = (pivot.y - ub) * scale.y;
    double d = (db - pivot.y) * scale.y;

    // Avoid division by zero
    double lrSum = l + r;
    double udSum = u + d;

    // Calculate expanded bounds
    int fl = static_cast<int>((scale.x - 1) * l / (lrSum > 1e-9 ? lrSum : l));
    int fr = static_cast<int>((scale.x - 1) * (1 - l / (lrSum > 1e-9 ? lrSum : r)));
    int fu = static_cast<int>((scale.y - 1) * u / (udSum > 1e-9 ? udSum : u));
    int fd = static_cast<int>((scale.y - 1) * (1 - u / (udSum > 1e-9 ? udSum : d)));
    
    if (lrSum <= 1e-9) fl = fr = 0;
    if (udSum <= 1e-9) fu = fd = 0;
    
    return std::make_tuple(fl, fr, fu, fd);
}

std::tuple<int, int, int, int> SpriteRenderer::GetPivotBounds() {
    Vector2 pivot = this->GetPivot();
    std::string shape = this->shape;

    auto transform = parent->GetComponent<Transform>();
    Vector3 scale = transform->scale;

    

    Vector2 size(spriteWidth, spriteHeight);

    // Get rotated corner positions
    Vector2 leftUp = RotatePoint(0, 0);
    Vector2 leftDown = RotatePoint(0, size.y - 1);
    Vector2 rightDown = RotatePoint(size.x - 1, size.y - 1);
    Vector2 rightUp = RotatePoint(size.x - 1, 0);

    // Compute bounding box
    double lb = std::min({leftUp.x, leftDown.x, rightDown.x, rightUp.x});
    double rb = std::max({leftUp.x, leftDown.x, rightDown.x, rightUp.x});
    double ub = std::min({leftUp.y, leftDown.y, rightDown.y, rightUp.y});
    double db = std::max({leftUp.y, leftDown.y, rightDown.y, rightUp.y});
    
    // Scale bounds
    double l = (pivot.x - lb) * scale.x;
    double r = (rb - pivot.x) * scale.x;
    double u = (pivot.y - ub) * scale.y;
    double d = (db - pivot.y) * scale.y;

    // Avoid division by zero
    double lrSum = l + r;
    double udSum = u + d;

    // Calculate expanded bounds
    int fr = static_cast<int>((scale.x - 1) * r / (lrSum > 1e-9 ? lrSum : 1));
    int fl = static_cast<int>((scale.x - 1) - fr);
    int fd = static_cast<int>((scale.y - 1) * d / (udSum > 1e-9 ? udSum : 1));
    int fu = static_cast<int>((scale.y - 1) - fd);

    if (lrSum <= 1e-9) fl = fr = 0;
    if (udSum <= 1e-9) fu = fd = 0;

    return std::make_tuple((int)l + fl, (int)r + fr, (int)u + fu, (int)d + fd);
}


std::string SpriteRenderer::GetCellString(int column, int line) {
    auto pivotBounds = GetPivotBounds();
    std::string shape;
    if (this->useMarkdown) {
        shape = ProcessMarkdown(shape);
    }
    if (this->spriteColor != Color::RESET) {
        shape = ToAnsiCode(spriteColor) + shape + "\033[0m";
    }
    auto transform = parent->GetComponent<Transform>();
    double rotation = transform->rotation;
    Vector3 scale = transform->scale;

    double radians = -rotation * (M_PI / 180.0);
    Vector2 pivot = this->GetPivot();

    int pivotX = static_cast<int>(round(pivot.x));
    int pivotY = static_cast<int>(round(pivot.y));

    column -= pivotX;
    line -= pivotY;

    int rotatedX = static_cast<int>(round(column * cos(radians) - line * sin(radians)));
    int rotatedY = static_cast<int>(round(column * sin(radians) + line * cos(radians)));

    rotatedX += pivotX;
    rotatedY += pivotY;

    #ifdef DEVELOPPER_DEBUG_MODE
      printf("Rotated crumb: %d %d, ", rotatedX, rotatedY);
      fflush(stdout);
    #endif

    int scaledX, scaledY;
    std::tuple<int, int, int, int> pvex = CalculatePivotExpansion();

    #ifdef DEVELOPPER_DEBUG_MODE
      printf("Pivot expansion: %d %d %d %d, ", std::get<0>(pvex), std::get<1>(pvex), std::get<2>(pvex), std::get<3>(pvex));
    #endif

    if (rotatedX <= pivotX + std::get<1>(pvex) && rotatedX >= pivotX - std::get<0>(pvex)) {
        scaledX = pivotX;
       // if(rotatedX != pivotX) return " ";
    } else if (rotatedX >= pivotX + std::get<1>(pvex)) {
        scaledX = static_cast<int>((rotatedX - pivotX - std::get<1>(pvex) - 1) / scale.x + 1 + pivotX);
        //if((rotatedX - pivotX - std::get<1>(pvex) - 1) % (int)scale.x) return " ";
        
    } else {
        scaledX = static_cast<int>((rotatedX - pivotX + std::get<0>(pvex) + 1) / scale.x - 1 + pivotX);
        //if((rotatedX - pivotX + std::get<0>(pvex) + 1) % (int)scale.x) return " ";
    }

    if (rotatedY <= pivotY + std::get<3>(pvex) && rotatedY >= pivotY - std::get<2>(pvex)) {
        scaledY = pivotY;
        //if(rotatedY != pivotY) return " ";
    } else if (rotatedY >= pivotY + std::get<3>(pvex)) {
        scaledY = static_cast<int>((rotatedY - pivotY - std::get<3>(pvex) - 1) / scale.y + 1 + pivotY);
        //if((rotatedY - pivotY - std::get<3>(pvex) - 1) % (int)scale.y) return " ";
    } else {
        scaledY = static_cast<int>((rotatedY - pivotY + std::get<2>(pvex) + 1) / scale.y - 1 + pivotY);
        //if((rotatedY - pivotY + std::get<2>(pvex) + 1) % (int)scale.y) return " ";
    }

    #ifdef DEVELOPPER_DEBUG_MODE
      printf("Scaled crumb: %d %d\n", scaledX, scaledY);
      fflush(stdout);
    #endif

    std::string currentLine;
    int currentLineIndex = 0;
    std::stringstream ss(cleanShape);

    while (std::getline(ss, currentLine, '\n')) {
        if (currentLineIndex == scaledY) {
            if (scaledX >= 0 && scaledX < static_cast<int>(StripAnsi(currentLine).size())) {
                if (ansiExtracted[currentLineIndex][scaledX].empty()) {
                    return std::string(1, currentLine[scaledX]) + ToAnsiCode(Color::RESET);
                } else {
                    return ansiExtracted[currentLineIndex][scaledX] + currentLine[scaledX] + ToAnsiCode(Color::RESET);
                }
            }
            return " ";
        }
        currentLineIndex++;
    }

    return " ";
}
std::string SpriteRenderer::getShape() {
  return shape;
}

void SpriteRenderer::setShape(std::string target) {
  shape = target;
  cleanShape = StripAnsi(ProcessMarkdown(shape));
  spriteHeight = 0;
  spriteWidth = 0;
  std::string line;
  ss.str(cleanShape);
  while (std::getline(ss, line, '\n')) {
      spriteHeight++;
      spriteWidth = std::max(spriteWidth, static_cast<int>(line.size()));
  }
  
  ansiExtracted = ExtractAnsi(shape);
}


    
