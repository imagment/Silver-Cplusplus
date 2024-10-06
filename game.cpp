// game.cpp 
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <atomic>
#include <cstdarg>
#include <ctime>
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

#include "game.hpp"

#define DEFAULT_TICK 0.1
#define WORLD_SIZE 100
#define WORLD_DEPTH 51
#define BUFFER_SIZE 5000


#define MAX_TOKEN_SIZE 1000



using namespace std;

mutex scriptMutex; 
unordered_set<string> currentPressedKeys;
unordered_set<string> previousPressedKeys;
atomic<bool> keepListening(true);
mutex keyMutex;
multimap<tuple<int, int, int>, mesh> sprites;
//multimap<pair<string,int>, tuple<int, int, int>> string_sprites;
map <string, objects> globalObjects;


vector<scene> scenes;


string out_of_bounds = "🟦";
string null_rep = "🧱";

int WorldX = 20, WorldY = 20;
int cursorPositionX = 0;
int cursorPositionY = 0;


Silver silver;


void Silver::Camera::cell(int c){
	silver.camera.cellSize=c;
}

void Silver::Camera::configCameraException(string o, string n){
	null_rep=n;
	out_of_bounds=o;
}


char Silver::Keyboard::getKey() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Disable canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Set non-blocking mode for input
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    // Check for key press
    ch = getchar();

    // Restore original terminal settings after checking for key
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    // If a key is pressed, return the key without echoing it to the console
    if (ch != EOF) {
    	if(ch>='a' && ch<='z') ch+='A'-'a';
    	keyBuffer=ch;
        return ch;  // Return detected key
    }
    keyBuffer='\0';
    return '\0'; // Return null character if no key was pressed
}






bool Silver::Keyboard::isKey(char key) {
    if (keyBuffer==key) {
        return true;
    }
    return false;
}




void Silver::gotoxy(int x, int y )
{
    printf("\033[%d;%dH", y+1, x+1);
    //Use 'H' to handle strings as well as chars
}

Vec3 rotatePoint(Vec3 point, float angle) {
    float radians = angle * (M_PI / 180.0f);
    float cosA = cos(radians);
    float sinA = sin(radians);    
    return Vec3(static_cast<int>(round(point.x * cosA - point.y * sinA)), 
                 static_cast<int>(round(point.x * sinA + point.y * cosA)),0);
}


void Silver::draw(Vec3 pos, string c) {
    mesh p;
    p.object.position = pos;
    p.object.number = -214743648;
    p.shape = c;
    p.transparency = 0;

   sprites.insert({{pos.x, pos.y, pos.z}, p});
   
}
void Silver::placeSpray(string objectName, int number, int spawns, Vec3 center, int range) {
    if (spawns <= 0 || globalObjects.count(objectName) == 0) {
        return;
    }

    auto objIt = globalObjects.find(objectName);
    if (objIt != globalObjects.end()) {
        // Seed the random number generator
        srand(static_cast<unsigned int>(time(nullptr)));

        for (int i = 0; i < spawns; ++i) {
            // Generate random offsets within the specified range
            int offsetX = rand() % (2 * range + 1) - range; // Range: [-range, range]
            int offsetY = rand() % (2 * range + 1) - range; // Range: [-range, range]

            // Create the new position
            Vec3 position = {center.x + offsetX, center.y + offsetY, center.z};

            // Create a mesh object and initialize it
            mesh X;
            X.object.position = position;
            X.object.number = number; // or any specific number logic you want
            X.object.name = objectName;
            X.shape = objIt->second.shape;
            X.transparency = objIt->second.transparency;
            X.anim = objIt->second.anim;
            X.Components = objIt->second.Components; // Assign the Components member

            // Draw the object
            sprites.insert({{position.x, position.y, position.z},X}); // Add to sprites
            
        }
    }
}

void Silver::placeExactSpray(string objectName, int number, int count, Vec3 startPosition) {
    if (count <= 0 || globalObjects.count(objectName) == 0) {
        return;
    }

    auto objIt = globalObjects.find(objectName);
    if (objIt != globalObjects.end()) {
        for (int i = 0; i < count; ++i) {
            // Create the position for each object
            Vec3 position = {startPosition.x, startPosition.y, startPosition.z + i}; // Stacking in the z-direction

            // Place the object at the new position
            mesh X;
            X.object.position = position;
            X.object.number = number; // Adjust if necessary
            X.object.name = objectName;
            X.shape = objIt->second.shape;
            X.transparency = objIt->second.transparency;
            X.anim = objIt->second.anim;
            X.Components = objIt->second.Components;

            // Place the object in the sprites map
            sprites.insert({{position.x, position.y, position.z},X});
        }
    }
}

void Silver::drawRectangle(Vec3 topLeft, int width, int height, string c) {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            Vec3 pos = {topLeft.x + i, topLeft.y + j, topLeft.z};
            draw(pos, c);
        }
    }
}

void Silver::drawRectangleHollow(Vec3 topLeft, int width, int height, string c) {
    for (int i = 0; i < width; ++i) {
        draw({topLeft.x + i, topLeft.y, topLeft.z}, c);                       
        draw({topLeft.x + i, topLeft.y + height - 1, topLeft.z}, c);            
    }

    for (int j = 1; j < height - 1; ++j) {
        draw({topLeft.x, topLeft.y + j, topLeft.z}, c);                         
        draw({topLeft.x + width - 1, topLeft.y + j, topLeft.z}, c);            
    }
}


void Silver::drawCircle(Vec3 center, int radius, string c) {
    for (int y = center.y - radius; y <= center.y + radius; ++y) {
        for (int x = center.x - radius; x <= center.x + radius; ++x) {
            int dx = x - center.x;
            int dy = y - center.y;

            
            if (dx * dx + dy * dy <= radius * radius) {
                draw({x, y, center.z}, c);
            }
        }
    }
}

void Silver::drawLine(Vec3 start, Vec3 end, string c) {
    int x1 = start.x;
    int y1 = start.y;
    int x2 = end.x;
    int y2 = end.y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1; // Step in x direction
    int sy = (y1 < y2) ? 1 : -1; // Step in y direction

    int err = dx - dy;

    while (true) {
        draw({x1, y1, start.z}, c); // Draw the pixel at (x1, y1)

        // Check if we reached the end point
        if (x1 == x2 && y1 == y2) break;

        if (err*2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (err*2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}


void Silver::drawCircleHollow(Vec3 center, int radius, string c) {
    for (int y = center.y - radius; y <= center.y + radius; ++y) {
        for (int x = center.x - radius; x <= center.x + radius; ++x) {
            int dx = x - center.x;
            int dy = y - center.y;
            int distanceSquared = dx * dx + dy * dy;

         
            if (distanceSquared >= (radius - 1) * (radius - 1) && distanceSquared <= radius * radius) {
                draw({x, y, center.z}, c);
            }
        }
    }
}


void Silver::Camera::setCam3(Vec3 pos, Vec3 scale) {
    silver.camera.CameraPos=pos;
    silver.camera.CameraScale=scale;
}

void Silver::Camera::setCam2(Vec3 pos, Vec3 scale) {
    silver.camera.CameraPos={pos.x,pos.y,silver.camera.CameraPos.z};
    silver.camera.CameraScale={pos.x,pos.y,silver.camera.CameraScale.z};
}


void Silver::Camera::photo() {
    float radians = silver.camera.cameraRotation * (M_PI / 180.0);
    float cosTheta = cos(radians);
    float sinTheta = sin(radians);

    for (int j = 0; j < silver.camera.CameraScale.y; ++j) {
        for (int i = 0; i < silver.camera.CameraScale.x; ++i) {
            bool flag = true;
            mesh s;

            int dx = i - silver.camera.CameraScale.x / 2;
            int dy = j - silver.camera.CameraScale.y / 2;

            int rotatedX = round(cosTheta * dx + sinTheta * dy + silver.camera.CameraPos.x);
            int rotatedY = round(-sinTheta * dx + cosTheta * dy + silver.camera.CameraPos.y);

            if (silver.camera.isFlippedHorizonal == -1) {
                rotatedX = WorldX - rotatedX - 1;
            }
            if (isFlippedVertical == -1) {
                rotatedY = WorldY - rotatedY - 1;
            }

            
		bool spriteFound = false;
		for (int z = silver.camera.CameraPos.z+silver.camera.CameraScale.z/2-1; z >= silver.camera.CameraPos.z+(-silver.camera.CameraScale.z+silver.camera.CameraScale.z/2); --z) {
		    auto it = sprites.find({rotatedX, rotatedY, z});
		    if (it != sprites.end() && it->second.transparency == 0) {
		        cout << it->second.shape;
		        spriteFound = true;
		        break;
		    }
		}

		if (!spriteFound) {
			if (rotatedX < 0 || rotatedY < 0 || rotatedX >= WorldX || rotatedY >= WorldY) cout << out_of_bounds;
			 
		        else cout << null_rep;
		} 
	    }
        
        cout << endl;
    }
}

void Silver::setWorldBounds(Vec3 world){
	WorldX=world.x;
	WorldY=world.y;
}

vector <int> Silver::Seek(string objectName) {
	vector <int> numbers;
	for(auto objIt : sprites) {
		if(objIt.second.object.name==objectName){
			numbers.push_back(objIt.second.object.number);
		}
	}
	return numbers;
}

vector<vector<string>> Silver::Camera::gPhoto() {
    vector<vector<string>> output(silver.camera.CameraScale.y, vector<string>(silver.camera.CameraScale.x));

    float radians = silver.camera.cameraRotation * (M_PI / 180.0);
    float cosTheta = cos(radians);
    float sinTheta = sin(radians);

    for (int j = 0; j < silver.camera.CameraScale.y; ++j) {
        for (int i = 0; i < silver.camera.CameraScale.x; ++i) {
            bool flag = true;

            int dx = i - silver.camera.CameraScale.x / 2;
            int dy = j - silver.camera.CameraScale.y / 2;

            int rotatedX = round(cosTheta * dx + sinTheta * dy + silver.camera.CameraPos.x);
            int rotatedY = round(-sinTheta * dx + cosTheta * dy + silver.camera.CameraPos.y);

            if (silver.camera.isFlippedHorizonal == -1) {
                rotatedX = WorldX - rotatedX - 1;
            }
            if (silver.camera.isFlippedVertical == -1) {
                rotatedY = WorldY - rotatedY - 1;
            }

           
                bool spriteFound = false;
                for (int z = silver.camera.CameraPos.z+silver.camera.CameraScale.z/2-1; z >= silver.camera.CameraPos.z+(-silver.camera.CameraScale.z+silver.camera.CameraScale.z/2); --z) {
                    auto it = sprites.find({rotatedX, rotatedY, z});
                    if (it != sprites.end() && it->second.transparency == 0) {
                        output[j][i] = it->second.shape;
                        spriteFound = true;
                        break;
                    }
                }

                if (!spriteFound) {
                   if (rotatedX < 0 || rotatedY < 0 || rotatedX >= WorldX || rotatedY >= WorldY) {
		        output[j][i] = out_of_bounds;
		   } else { 
                        output[j][i] = null_rep;
                   }
                } 
            }
        
    }

    return output;
}

static map<tuple<int, int>, string> lastFrame;
bool isFirst=true;
mutex bufferMutex;


void Silver::Camera::printCam() {
    if(isFirst) {silver.setRawMode(); silver.clear();}
    // Initialize the buffer with null_rep
    vector<vector<string>> buffer(silver.camera.CameraScale.y, vector<string>(silver.camera.CameraScale.x, null_rep));

    float radians = silver.camera.cameraRotation * (M_PI / 180.0);
    float cosTheta = cos(radians);
    float sinTheta = sin(radians);

    // Precompute camera offsets
    int centerX = silver.camera.CameraScale.x / 2;
    int centerY = silver.camera.CameraScale.y / 2;

    // Render the entire camera area
    for (int j = 0; j < silver.camera.CameraScale.y; ++j) {
        for (int i = 0; i < silver.camera.CameraScale.x; ++i) {
            int dx = i - centerX;
            int dy = j - centerY;

            // Precompute rotated coordinates
            int rotatedX = round(cosTheta * dx + sinTheta * dy + silver.camera.CameraPos.x);
            int rotatedY = round(-sinTheta * dx + cosTheta * dy + silver.camera.CameraPos.y);

            if (silver.camera.isFlippedHorizonal == -1) {
                rotatedX = WorldX - rotatedX - 1;
            }
            if (silver.camera.isFlippedVertical == -1) {
                rotatedY = WorldY - rotatedY - 1;
            }

            // Default shape is out of bounds
            string currentShape = (rotatedX < 0 || rotatedY < 0 || rotatedX >= WorldX || rotatedY >= WorldY) 
                                    ? out_of_bounds 
                                    : null_rep;

            // Find sprite at this position
            for (int z = silver.camera.CameraPos.z+silver.camera.CameraScale.z/2-1; z >= silver.camera.CameraPos.z+(-silver.camera.CameraScale.z+silver.camera.CameraScale.z/2); --z) {
                auto it = sprites.find({rotatedX, rotatedY, z});
                if (it != sprites.end() && it->second.transparency == 0) {
                    currentShape = it->second.shape;
                    break;
                }
            }

            // Store current shape in the buffer
            buffer[j][i] = currentShape;
        }
    }

    // Output the buffer, only update changes
    for (int j = 0; j < silver.camera.CameraScale.y; ++j) {
        for (int i = 0; i < silver.camera.CameraScale.x; ++i) {
            auto currentFrame = make_tuple(i, j);
            if(i==cursorPositionX && j==cursorPositionY) {
                	silver.gotoxy(i*silver.camera.cellSize, j);
                	cout << "↖️" << flush;
                	lastFrame[currentFrame] = "↖️";
                	continue;
            }
            if (isFirst || lastFrame[currentFrame] != buffer[j][i]) {
                silver.gotoxy(i*silver.camera.cellSize, j);
                
                if (buffer[j][i].size() > silver.camera.cellSize * 4) {
		    // Print only the first 'silver.camera.cellSize' characters
		    cout << buffer[j][i].substr(0, silver.camera.cellSize) << flush;
	

		    // Store only the first 'silver.camera.cellSize' characters in lastFrame
		    lastFrame[currentFrame] = buffer[j][i];
		}

                else {
                	cout << buffer[j][i] << flush;
                	lastFrame[currentFrame] = buffer[j][i];
                }
            }
        }
    }

    // First frame is rendered
    isFirst = false;

    // Handle sign messages
    bool sign = false;
    string signMessage, signIcon, objectName;

    for (int j = 0; j < silver.camera.CameraScale.y; ++j) {
        for (int i = 0; i < silver.camera.CameraScale.x; ++i) {
            int dx = i - centerX;
            int dy = j - centerY;

            // Precompute rotated coordinates
            int rotatedX = round(cosTheta * dx + sinTheta * dy + silver.camera.CameraPos.x);
            int rotatedY = round(-sinTheta * dx + cosTheta * dy + silver.camera.CameraPos.y);

            if (isFlippedHorizonal == -1) {
                rotatedX = WorldX - rotatedX - 1;
            }
            if (silver.camera.isFlippedVertical == -1) {
                rotatedY = WorldY - rotatedY - 1;
            }

            if (rotatedX >= 0 && rotatedY >= 0 && rotatedX < WorldX && rotatedY < WorldY) {
                for (int z = silver.camera.CameraPos.z+silver.camera.CameraScale.z/2-1; z >= silver.camera.CameraPos.z+(-silver.camera.CameraScale.z+silver.camera.CameraScale.z/2); --z) {
                    auto it = sprites.find({rotatedX, rotatedY, z});
                    if (it != sprites.end() && it->second.transparency == 0 && !it->second.Components.signMessage.empty()) {
                        signMessage = it->second.Components.signMessage;
                        sign = true;
                        signIcon = it->second.shape;
                        objectName = it->second.object.name;
                        break;
                    }
                }
            }
        }
    }

    // Display sign message if found
    if (sign) {
        cout << "\n\n*************************************\n"
             << signIcon << string(silver.camera.cellSize, ' ') << objectName << endl
             << signMessage << endl;
    }
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

void Silver::clean(int x1, int y1, int x2, int y2) {
    auto it = sprites.begin();
    while (it != sprites.end()) {
        auto key = it->first;
        int x = get<0>(key);
        int y = get<1>(key);
        if (x >= min(x1, x2) && x <= max(x1, x2) &&
            y >= min(y1, y2) && y <= max(y1, y2)) {
            it = sprites.erase(it);  
        } else {
            ++it;
        }
    }
}

void Silver::loadChunk(const string &file) {
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
            if (sscanf(buffer.c_str() + sizeof("📷") - 1, "%d %d %d %d %d %d", &A, &B, &C, &D, &E, &F) == 6) {
                Vec3 position = {A, B, C};
                Vec3 scale = {D,E,F};
                
                silver.camera.setCam3(position, scale);
                type = 1;
            }
        } else if (buffer.rfind("🗺️", 0) == 0) {
            char context[BUFFER_SIZE];
            int coord1, coord2, coord3;
            if (sscanf(buffer.c_str() + sizeof("🗺️") - 1, " \"%[^\"]\" %d %d %d", context, &coord1, &coord2, &coord3) == 4) {
            	
                silver.draw(Vec3(coord1, coord2, coord3), context);
            }
        } else if (buffer.rfind("💥", 0) == 0) {
            int x1, y1, x2, y2;
            if (sscanf(buffer.c_str() + sizeof("💥") - 1, " %d %d %d %d", &x1, &y1, &x2, &y2) == 4) {
                clean(x1, y1, x2, y2);
            }
        }
    }
}

void Silver::Camera::moveCamera(Vec3 V){
    silver.camera.CameraPos.x += V.x;
    silver.camera.CameraPos.y += V.y;
}

void Silver::Camera::shakeCameraOnce(float intensity) {
    float offsetX = intensity * (rand() % 100 / 100.0f - 0.5f);  
    float offsetY = intensity * (rand() % 100 / 100.0f - 0.5f);

    silver.camera.CameraPos.x += (int)offsetX;
    silver.camera.CameraPos.y += (int)offsetY;
}

void Silver::Camera::shakeCamera(float intensity, int shakes, float delayBetweenShakes) {
    float originalX = silver.camera.CameraPos.x;
    float originalY = silver.camera.CameraPos.y;

    for (int i = 0; i < shakes; ++i) {
        float offsetX = intensity * (rand() % 100 / 100.0f - 0.5f);  
        float offsetY = intensity * (rand() % 100 / 100.0f - 0.5f);

        silver.camera.CameraPos.x = originalX + (int)offsetX;
        silver.camera.CameraPos.y = originalY + (int)offsetY;

        silver.wait(delayBetweenShakes);
    }

    silver.camera.CameraPos.x = originalX;
    silver.camera.CameraPos.y = originalY;
}

pair <int,int> Silver::getConsoleSize() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int width = w.ws_col;
    int height = w.ws_row;
    return {width,height};
}

Vec2 Silver::getConsoleCenter() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int width = w.ws_col;
    int height = w.ws_row;
    return Vec2(width-width/2,height-height/2);
}


void Silver::Camera::zoomCamera(Vec3 V){
    silver.camera.CameraScale.x += V.x;
    silver.camera.CameraScale.y += V.y;
    if (silver.camera.CameraScale.x < 0) silver.camera.CameraScale.x = 0;
    if (silver.camera.CameraScale.y < 0) silver.camera.CameraScale.y = 0;
}



atomic<bool> VMouse = false; // To stop the thread when needed

void VMouseOn(int l, int r, int u, int d, int c) {
    
    while (!VMouse) {
   	    char det=silver.keyboard.getKey();
            if (det==l) {
		cursorPositionX--; // Move cursor left
	    }
	    if (det==r) {
		cursorPositionX++; // Move cursor right
	    }
	    if (det==u) {
		cursorPositionY--; // Move cursor up
	    }
	    if (det==d) {
		cursorPositionY++; // Move cursor down
	    }
	    if (det==c) {

		cout << "Click detected at (" << cursorPositionX << ", " << cursorPositionY << ")" << endl;
	    }
	    this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void Silver::startVMouse(int l, int r, int u, int d, int c)  {
    VMouse = false; 
    thread vmouseThread(VMouseOn, l, r, u, d, c);
    vmouseThread.detach(); 
}

void Silver::stopVMouse() {
    VMouse= true; 
}



void startLoading() {
    silver.clear();
    printf("--------------------------------\n");
    printf(". . . . . . . . . . . . . . . . \n");
    printf("--------------------------------\n");
    silver.gotoxy(0,1);
    cout << "#" << flush;
}
/*
vector<string> splitLines(const string &s) {
    vector<string> lines;
    string temp;
    for (char c : s) {
        if (c == '\n') {
            lines.push_back(temp);
            temp.clear();
        } else {
            temp += c;
        }
    }
    if (!temp.empty()) {
        lines.push_back(temp);  
    }
    return lines;
}

void printCenter(string s, int x, int y) {
    vector<string> lines = splitLines(s);  

    int startY = y - (lines.size() / 2);   

    for (int i = 0; i < lines.size(); ++i) {
        int len = lines[i].length();
        int startX = x - (len / 2);       
        silver.gotoxy(startX, startY + i);        
        cout << lines[i];                 
    }
}

*/
/*

  _________.__.__                      
 /   _____/|__|  |___  __ ___________  
 \_____  \ |  |  |\  \/ // __ \_  __ \ 
 /        \|  |  |_\   /\  ___/|  | \/ 
/_______  /|__|____/\_/  \___  >__|    
        \/                   \/     
    In silver, the worlds deliver   
  ________________________________
 /####################            \
 ==================================
 
         Loading.... 65%
        Version Num: 1002
 
*/


zone RectangleZone(Vec3 first, Vec3 second){
    zone z;
    z.rect[0].x1=first.x;
    z.rect[0].y1=first.y;
    z.rect[0].x2=second.x;
    z.rect[0].y2=second.y;

    return z;
}

void Silver::createObject(const string name, const string shape){
    if (globalObjects.count(name) > 0) {
        return; 
    }

    objects A;
    A.name = name;
    A.shape = shape;
    A.transparency = 0;
    globalObjects[name] = A;
}

void Silver::place(string objectName, int number, Vec3 position) {
    if (number < 0) {
        number = 0;
    }
    if (globalObjects.count(objectName) == 0){
        return;
    }

    auto objIt = globalObjects.find(objectName);
    if (objIt != globalObjects.end()) {
        mesh X;
        X.object.position = position;
        X.object.number = number;
        X.object.name = objectName;
        X.shape = objIt->second.shape;
        X.transparency = objIt->second.transparency;
        X.anim = objIt->second.anim;
        X.Components = objIt->second.Components;

        sprites.insert({{position.x, position.y, position.z},X});
    }
}



void Silver::put(string objectName, Vec3 position) {
    auto objIt = globalObjects.find(objectName);
    if (objIt != globalObjects.end()) {
        mesh X;
        X.object.position = position;
        X.object.number = 0;
        X.object.name = objectName;
        X.shape = objIt->second.shape;
        X.transparency = objIt->second.transparency;
        X.anim = objIt->second.anim;
        
        
        X.Components = objIt->second.Components;

        sprites.insert({{position.x, position.y, position.z},X});
    }
}

void Silver::applyScript(const string& objectName, int number, const string& script) {
    if (number == origin) {
        auto it = globalObjects.find(objectName);
        if (it != globalObjects.end()) {
            it->second.scripts += script + "\n";
        }
    } else if (number == all) {
        for (auto& entry : sprites) {
            if (entry.second.object.name == objectName) {
                entry.second.scripts += script + "\n";
            }
        }
    } else {
        for (auto& entry : sprites) {
            if (entry.second.object.name == objectName && entry.second.object.number == number) {
                entry.second.scripts += script + "\n";
                break; 
            }
        }
    }
}


atomic<bool> isRunningCam(false);

void Silver::Camera::startVideo(int FPS) {
  isRunningCam = true; // Set the flag to true to start the loop
  thread videoThread([&FPS] { 
    while (isRunningCam) {
      silver.camera.printCam();
      this_thread::sleep_for(chrono::milliseconds(static_cast<int>(1000.0f / FPS)));
    }
  });
  videoThread.detach();
}

void Silver::Camera::endVideo() {
  isRunningCam = false; // Set the flag to false to stop the loop
}
void Silver::Camera::restartVideo() {
  isRunningCam = true; // Set the flag to false to stop the loop
}
void Silver::wait(float time){
    this_thread::sleep_for(chrono::milliseconds(static_cast<int>(time)));
}

void Silver::applyComponent(string object, int number, string component, ...) {
    va_list args;
    va_start(args, component);

    if (component == "sign") {
        string text = va_arg(args, char*);
        if (number == origin) {
            auto it = globalObjects.find(object);
            if (it != globalObjects.end()) {
                it->second.Components.signMessage = text;
            }
        } else if (number == all) {
            for (auto &entry : sprites) {
                if (entry.second.object.name == object) {
                    entry.second.Components.signMessage = text;
                }
            }
        } else {
            for (auto &entry : sprites) {
                if (entry.second.object.name == object && entry.second.object.number == number) {
                    entry.second.Components.signMessage = text;
                }
            }
        }
    } 
    else if (component == "sign_reader") { //probably later
        string key = va_arg(args, char*);

        if (number == origin) {
            auto it = globalObjects.find(object);
            if (it != globalObjects.end()) {
                it->second.Components.readerKey = key;
            }
        } else if (number == all) {
            for (auto &obj : sprites) {
                if (obj.second.object.name == object) {
                    obj.second.Components.readerKey = key;
                }
            }
        } else {
            for (auto &obj : sprites) {
                if (obj.second.object.name == object && obj.second.object.number == number) {
                    obj.second.Components.readerKey = key;
                }
            }
        }
    }

    va_end(args);
}

vector<string> Silver::spriteAt3(Vec3 pos) {
    vector<string> result;

    // Using range-based iteration to find all elements with matching position
    auto range = sprites.equal_range({pos.x, pos.y, pos.z});
    for (auto it = range.first; it != range.second; ++it) {
        result.push_back(it->second.object.name); // Assuming the strings are stored as second in pair
    }

    return result;
}
vector<string> Silver::spriteAt2(Vec3 pos) {
    vector<string> names;

    // Ensure position is within bounds
    if (pos.x >= 0 && pos.y >= 0 && pos.x < WorldX && pos.y < WorldY) {
        // Iterate over all sprites
        for (auto& entry : sprites) {
            if (entry.second.object.position.x == pos.x && entry.second.object.position.y == pos.y) {
                // Add the name of the object to the vector
                names.push_back(entry.second.object.name);
            }
        }
    }

    return names;
}

void Silver::setObjectXY(const string& name, const variant<int, vector<int>>& number, Vec3 pos) {
    int x=pos.x;
    int y=pos.y;
    if (holds_alternative<int>(number)) {
            int singleNumber = get<int>(number);
	    if (singleNumber == origin){
		return; 
	    }
	    
	    else if (singleNumber == all){
		for (auto& entry : sprites){
		    if (entry.second.object.name == name){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()) {
		            mesh X = it->second;
		            // Keep the original z value and only update x and y
		            int z = X.object.position.z;
		            X.object.position = Vec3(x, y, z);  
		            sprites.erase(it); 
		            sprites.insert({{x,y,z},X});
		        }
		    }
		}
	    }
	    else {
		for (auto& entry : sprites){
		    if (entry.second.object.name == name && entry.second.object.number == singleNumber){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()){
		            mesh X = it->second;
		            // Keep the original z value and only update x and y
		            int z = X.object.position.z;
		            X.object.position = Vec3(x, y, z);
		            sprites.erase(it);
		            sprites.insert({{x,y,z},X});
		        }
		    }
		}
	    }
	}
	else {
		const vector<int>& numbers = get<vector<int>>(number);
		for (int num : numbers) {
		    // Handle each number in the array
		    for (auto& entry : sprites) {
		        if (entry.second.object.name == name && entry.second.object.number == num) {
		            auto it = sprites.find(entry.first);
		            if (it != sprites.end()) {
		                mesh X = it->second;
		                int z = X.object.position.z; // Keep the original z value
		                X.object.position = Vec3{x, y, z};  
		                sprites.erase(it);
		                sprites.insert({{x, y, z}, X});
		            }
		        }
		    }
		}
	}
}

void Silver::setObjectPosition(const string& name, const variant<int, vector<int>>& number, Vec3 pos) {
    int x=pos.x;
    int y=pos.y;
    int z=pos.z;
    if (holds_alternative<int>(number)) {
            int singleNumber = get<int>(number);
	    if (singleNumber == origin){
		return; 
	    }
	    
	    else if (singleNumber == all){
		for (auto& entry : sprites){
		    if (entry.second.object.name == name){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()) {
		            mesh X = it->second;

		            X.object.position = Vec3(x, y, z);  
		            sprites.erase(it); 
		            sprites.insert({{x,y,z},X});
		        }
		    }
		}
	    }
	    else {
		for (auto& entry : sprites){
		    if (entry.second.object.name == name && entry.second.object.number == singleNumber){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()){
		            mesh X = it->second;

		            X.object.position = Vec3(x, y, z);
		            sprites.erase(it);
		            sprites.insert({{x,y,z},X});
		        }
		    }
		}
	    }
	}
	else {
		const vector<int>& numbers = get<vector<int>>(number);
		for (int num : numbers) {
		    // Handle each number in the array
		    for (auto& entry : sprites) {
		        if (entry.second.object.name == name && entry.second.object.number == num) {
		            auto it = sprites.find(entry.first);
		            if (it != sprites.end()) {
		                mesh X = it->second;

		                X.object.position = Vec3{x, y, z};  
		                sprites.erase(it);
		                sprites.insert({{x, y, z}, X});
		            }
		        }
		    }
		}
	}
}


void Silver::moveObjectPosition(const string& name, const variant<int, vector<int>>& number, Vec3 pos) {
    int x=pos.x;
    int y=pos.y;
    int z=pos.z;
    if (holds_alternative<int>(number)) {
            int singleNumber = get<int>(number);
	    if (singleNumber == origin){
		return; 
	    }
	    
	    else if (singleNumber == all){
		for (auto& entry : sprites){
		    if (entry.second.object.name == name){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()) {
		            mesh X = it->second;

		            X.object.position += Vec3(x, y, z);  
		            sprites.erase(it); 
		            sprites.insert({{X.object.position.x, X.object.position.y, X.object.position.z},X});
		        }
		    }
		}
	    }
	    else {
		for (auto& entry : sprites){
		    if (entry.second.object.name == name && entry.second.object.number == singleNumber){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()){
		            mesh X = it->second;

		            X.object.position += Vec3(x, y, z);
		            sprites.erase(it);
		            sprites.insert({{X.object.position.x, X.object.position.y, X.object.position.z},X});
		        }
		    }
		}
	    }
	}
	else {
		const vector<int>& numbers = get<vector<int>>(number);
		for (int num : numbers) {
		    // Handle each number in the array
		    for (auto& entry : sprites) {
		        if (entry.second.object.name == name && entry.second.object.number == num) {
		            auto it = sprites.find(entry.first);
		            if (it != sprites.end()) {
		                mesh X = it->second;

		                X.object.position += Vec3{x, y, z};  
		                sprites.erase(it);
		                sprites.insert({{x, y, z}, X});
		            }
		        }
		    }
		}
	}
}





void Silver::moveObjectX(const string& name, const variant<int, vector<int>>& number, int x_offset) {
    int x=x_offset;

    if (holds_alternative<int>(number)) {
            int singleNumber = get<int>(number);
	    if (singleNumber == origin){
		return; 
	    }
	    
	    else if (singleNumber == all){
		for (auto& entry : sprites){
		    if (entry.second.object.name == name){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()) {
		            mesh X = it->second;
			   
		            X.object.position += Vec3(x, 0, 0);  
		            sprites.erase(it); 
		            sprites.insert({{X.object.position.x, X.object.position.y, X.object.position.z},X});
		        }
		    }
		}
	    }
	    else {
		for (auto& entry : sprites){
		    if (entry.second.object.name == name && entry.second.object.number == singleNumber){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()){
		            mesh X = it->second;

		            X.object.position += Vec3(x, 0, 0);
		            sprites.erase(it);
		            sprites.insert({{X.object.position.x, X.object.position.y, X.object.position.z},X});
		        }
		    }
		}
	    }
	}
	else {
		const vector<int>& numbers = get<vector<int>>(number);
		for (int num : numbers) {
		    // Handle each number in the array
		    for (auto& entry : sprites) {
		        if (entry.second.object.name == name && entry.second.object.number == num) {
		            auto it = sprites.find(entry.first);
		            if (it != sprites.end()) {
		                mesh X = it->second;

		                X.object.position += Vec3{x, 0, 0};  
		                sprites.erase(it);
		                sprites.insert({{X.object.position.x, X.object.position.y, X.object.position.z}, X});
		            }
		        }
		    }
		}
	}
}


void Silver::moveObjectY(const string& name, const variant<int, vector<int>>& number, int y_offset) {
    int y=y_offset;

    if (holds_alternative<int>(number)) {
            int singleNumber = get<int>(number);
	    if (singleNumber == origin){
		return; 
	    }
	    
	    else if (singleNumber == all){
		for (auto& entry : sprites){
		    if (entry.second.object.name == name){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()) {
		            mesh X = it->second;
			   
		            X.object.position += Vec3(0, y, 0);  
		            sprites.erase(it); 
		            sprites.insert({{X.object.position.x, X.object.position.y, X.object.position.z},X});
		        }
		    }
		}
	    }
	    else {
		for (auto& entry : sprites){
		    if (entry.second.object.name == name && entry.second.object.number == singleNumber){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()){
		            mesh X = it->second;

		            X.object.position += Vec3(0, y, 0);
		            sprites.erase(it);
		            sprites.insert({{X.object.position.x, X.object.position.y, X.object.position.z},X});
		        }
		    }
		}
	    }
	}
	else {
		const vector<int>& numbers = get<vector<int>>(number);
		for (int num : numbers) {
		    // Handle each number in the array
		    for (auto& entry : sprites) {
		        if (entry.second.object.name == name && entry.second.object.number == num) {
		            auto it = sprites.find(entry.first);
		            if (it != sprites.end()) {
		                mesh X = it->second;

		                X.object.position += Vec3{0, y, 0};  
		                sprites.erase(it);
		                sprites.insert({{X.object.position.x, X.object.position.y, X.object.position.z}, X});
		            }
		        }
		    }
		}
	}
}
void Silver::setObjectX(const string& name, const variant<int, vector<int>>& number, Vec3 pos) {
    int x=pos.x;
 
    if (holds_alternative<int>(number)) {
            int singleNumber = get<int>(number);
	    if (singleNumber == origin){
		return; 
	    }
	    
	    else if (singleNumber == all){
		for (auto& entry : sprites){
		    if (entry.second.object.name == name){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()) {
		            mesh X = it->second;
		            // Keep the original z value and only update x and y
		            int y = X.object.position.y;
		            int z = X.object.position.z;
		            X.object.position = Vec3(x, y, z);  
		            sprites.erase(it); 
		            sprites.insert({{x,y,z},X});
		        }
		    }
		}
	    }
	    else {
		for (auto& entry : sprites){
		    if (entry.second.object.name == name && entry.second.object.number == singleNumber){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()){
		            mesh X = it->second;
		            // Keep the original z value and only update x and y
		            int y = X.object.position.y;
		            int z = X.object.position.z;
		            X.object.position = Vec3(x, y, z);
		            sprites.erase(it);
		            sprites.insert({{x,y,z},X});
		        }
		    }
		}
	    }
	}
	else {
		const vector<int>& numbers = get<vector<int>>(number);
		for (int num : numbers) {
		    // Handle each number in the array
		    for (auto& entry : sprites) {
		        if (entry.second.object.name == name && entry.second.object.number == num) {
		            auto it = sprites.find(entry.first);
		            if (it != sprites.end()) {
		                mesh X = it->second;
		                int y = X.object.position.y;
		                int z = X.object.position.z; // Keep the original z value
		                X.object.position = Vec3{x, y, z};  
		                sprites.erase(it);
		                sprites.insert({{x, y, z}, X});
		            }
		        }
		    }
		}
	}
}


void Silver::glideObjectXY(string name, const variant <vector <int>, int>& number, Vec3 target_pos, float speed) {
    int target_x = target_pos.x;
    int target_y = target_pos.y;

    if(holds_alternative <int> (number)) {
    	    int singleNumber = get<int>(number);
	    if (singleNumber == origin) {
		return;
	    } 
	    else if (singleNumber == all) {
		bool anyMoving;
		
		// Move all objects with the given name one step at a time
		do {
		    anyMoving = false; // Reset moving flag for each loop iteration

		    for (auto& entry : sprites) {
		        if (entry.second.object.name == name) {
		            mesh& X = entry.second;

		            // Get current position
		            int start_x = X.object.position.x;
		            int start_y = X.object.position.y;

		            // Calculate the difference in X and Y
		            int deltaX = abs(target_x - start_x);
		            int deltaY = abs(target_y - start_y);
		            int signX = (start_x < target_x) ? 1 : -1;
		            int signY = (start_y < target_y) ? 1 : -1;

		            int err = deltaX - deltaY;

		            // If object hasn't reached its target, move it one step
		            if (start_x != target_x || start_y != target_y) {
		                anyMoving = true;  // At least one object is still moving

		                // Bresenham's line algorithm to calculate next step
		                int e2 = 2 * err;
		                if (e2 > -deltaY) {
		                    err -= deltaY;
		                    start_x += signX;
		                }
		                if (e2 < deltaX) {
		                    err += deltaX;
		                    start_y += signY;
		                }

		                // Move the object one step
		                silver.setObjectXY(name, entry.second.object.number, Vec2(start_x, start_y));
		            }
		        }
		    }

		    silver.wait(speed);  // Wait between each step for all objects

		} while (anyMoving);  // Continue loop while at least one object is still moving
	    } 
	    else {
		// Move a specific object
		for (auto& entry : sprites) {
		    if (entry.second.object.name == name && entry.second.object.number == singleNumber) {
		        mesh& X = entry.second;

		        // Get current position
		        int start_x = X.object.position.x;
		        int start_y = X.object.position.y;

		        // Calculate the difference in X and Y
		        int deltaX = abs(target_x - start_x);
		        int deltaY = abs(target_y - start_y);
		        int signX = (start_x < target_x) ? 1 : -1;
		        int signY = (start_y < target_y) ? 1 : -1;

		        int err = deltaX - deltaY;

		        // Loop until the object reaches its destination
		        while (1) {
		            silver.setObjectXY(name, entry.second.object.number, Vec2(start_x, start_y));
		            if (start_x == target_x && start_y == target_y) break;

		            // Bresenham's line algorithm to calculate next step
		            int e2 = 2 * err;
		            if (e2 > -deltaY) {
		                err -= deltaY;
		                start_x += signX;
		            }
		            if (e2 < deltaX) {
		                err += deltaX;
		                start_y += signY;
		            }

		            silver.wait(speed);  // Wait between each step
		        }
		    }
		}
	    }
	} else {
		bool anyMoving;
		const vector<int>& numbers = get<vector<int>>(number);
		
		// Move all objects with the given name one step at a time
		do {
		    anyMoving = false; // Reset moving flag for each loop iteration

		    for (auto& entry : sprites) {
		        if (entry.second.object.name == name && find(numbers.begin(), numbers.end(), entry.second.object.number) != numbers.end()) {
		            mesh& X = entry.second;

		            // Get current position
		            int start_x = X.object.position.x;
		            int start_y = X.object.position.y;

		            // Calculate the difference in X and Y
		            int deltaX = abs(target_x - start_x);
		            int deltaY = abs(target_y - start_y);
		            int signX = (start_x < target_x) ? 1 : -1;
		            int signY = (start_y < target_y) ? 1 : -1;

		            int err = deltaX - deltaY;

		            // If object hasn't reached its target, move it one step
		            if (start_x != target_x || start_y != target_y) {
		                anyMoving = true;  // At least one object is still moving

		                // Bresenham's line algorithm to calculate next step
		                int e2 = 2 * err;
		                if (e2 > -deltaY) {
		                    err -= deltaY;
		                    start_x += signX;
		                }
		                if (e2 < deltaX) {
		                    err += deltaX;
		                    start_y += signY;
		                }

		                // Move the object one step
		                silver.setObjectXY(name, entry.second.object.number, Vec2(start_x, start_y));
		            }
		        }
		    } 

		    silver.wait(speed);  // Wait between each step for all objects
	     }while (anyMoving);
	} 
}



void Silver::glideObjectX(string name, const variant<int, vector<int>>& number, int x_offset, float speed) {
    if (holds_alternative<int>(number)) {
            int singleNumber = get<int>(number);
	    if (singleNumber == origin) {
		return;
	    } 
	    else if (singleNumber == all) {
		for(int i=0; i<abs(x_offset); i++) {
			silver.wait(speed);
			for (auto& entry : sprites) {
			    if (entry.second.object.name == name) {
				mesh& X = entry.second;
				int dir;
				if(x_offset>0) dir=1;
				else dir=-1;
				
				silver.moveObjectX(X.object.name,X.object.number,dir);
				
			    }
			}
			
		} 
	    } 
	    else {
		for(int i=0; i<abs(x_offset); i++) {
			silver.wait(speed);
			for (auto& entry : sprites) {
			    if (entry.second.object.name == name && entry.second.object.number==singleNumber) {
				mesh& X = entry.second;
				int dir;
				if(x_offset>0) dir=1;
				else dir=-1;
				
				silver.moveObjectX(X.object.name,X.object.number,dir);
				
			    }
			}
			
		} 
	    }
    } else {
	    const vector<int>& numbers = get<vector<int>>(number);
	    for(int i = 0; i < abs(x_offset); i++) {
		silver.wait(speed);
		for (auto& entry : sprites) {
		    if (entry.second.object.name == name) {
		        mesh& X = entry.second;
		        
		        // Check if X.object.number is in the vector numbers
		        if (find(numbers.begin(), numbers.end(), X.object.number) != numbers.end()) {
		            int dir = (x_offset > 0) ? 1 : -1;
		            silver.moveObjectX(X.object.name, X.object.number, dir);
		        }
		    }
		}
	    } 
	}

}



void Silver::glideObjectY(string name, const variant<int, vector<int>>& number, int y_offset, float speed) {
    if (holds_alternative<int>(number)) {
            int singleNumber = get<int>(number);
	    if (singleNumber == origin) {
		return;
	    } 
	    else if (singleNumber == all) {
		for(int i=0; i<abs(y_offset); i++) {
			silver.wait(speed);
			for (auto& entry : sprites) {
			    if (entry.second.object.name == name) {
				mesh& X = entry.second;
				int dir;
				if(y_offset>0) dir=1;
				else dir=-1;
				
				silver.moveObjectY(X.object.name,X.object.number,dir);
				
			    }
			}
			
		} 
	    } 
	    else {
		for(int i=0; i<abs(y_offset); i++) {
			silver.wait(speed);
			for (auto& entry : sprites) {
			    if (entry.second.object.name == name && entry.second.object.number==singleNumber) {
				mesh& X = entry.second;
				int dir;
				if(y_offset>0) dir=1;
				else dir=-1;
				
				silver.moveObjectY(X.object.name,X.object.number,dir);
				
			    }
			}
			
		} 
	    }
    } else {
	    const vector<int>& numbers = get<vector<int>>(number);
	    for(int i = 0; i < abs(y_offset); i++) {
		silver.wait(speed);
		for (auto& entry : sprites) {
		    if (entry.second.object.name == name) {
		        mesh& X = entry.second;
		        
		        // Check if X.object.number is in the vector numbers
		        if (find(numbers.begin(), numbers.end(), X.object.number) != numbers.end()) {
		            int dir = (y_offset > 0) ? 1 : -1;
		            silver.moveObjectY(X.object.name, X.object.number, dir);
		        }
		    }
		}
	    } 
	}

}


void Silver::setObjectY(const string& name, const variant<int, vector<int>>& number, Vec3 pos) {
    int y=pos.y;
 
    if (holds_alternative<int>(number)) {
            int singleNumber = get<int>(number);
	    if (singleNumber == origin){
		return; 
	    }
	    
	    else if (singleNumber == all){
		for (auto& entry : sprites){
		    if (entry.second.object.name == name){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()) {
		            mesh X = it->second;
		            // Keep the original z value and only update x and y
		            int x = X.object.position.x;
		            int z = X.object.position.z;
		            X.object.position = Vec3(x, y, z);  
		            sprites.erase(it); 
		            sprites.insert({{x,y,z},X});
		        }
		    }
		}
	    }
	    else {
		for (auto& entry : sprites){
		    if (entry.second.object.name == name && entry.second.object.number == singleNumber){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()){
		            mesh X = it->second;
		            // Keep the original z value and only update x and y
		            int x = X.object.position.x;
		            int z = X.object.position.z;
		            X.object.position = Vec3(x, y, z);
		            sprites.erase(it);
		            sprites.insert({{x,y,z},X});
		        }
		    }
		}
	    }
	}
	else {
		const vector<int>& numbers = get<vector<int>>(number);
		for (int num : numbers) {
		    // Handle each number in the array
		    for (auto& entry : sprites) {
		        if (entry.second.object.name == name && entry.second.object.number == num) {
		            auto it = sprites.find(entry.first);
		            if (it != sprites.end()) {
		                mesh X = it->second;
		                int x = X.object.position.x;
		                int z = X.object.position.z; // Keep the original z value
		                X.object.position = Vec3{x, y, z};  
		                sprites.erase(it);
		                sprites.insert({{x, y, z}, X});
		            }
		        }
		    }
		}
	}
}

void Silver::moveObjectXY(const string& name, const variant<int, vector<int>>& number, Vec3 pos) {
    int x=pos.x;
    int y=pos.y;

    if (holds_alternative<int>(number)) {
            int singleNumber = get<int>(number);
	    if (singleNumber == origin){
		return; 
	    }
	    
	    else if (singleNumber == all){
		for (auto& entry : sprites){
		    if (entry.second.object.name == name){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()) {
		            mesh X = it->second;
			   
		            X.object.position += Vec3(x, y, 0);  
		            sprites.erase(it); 
		            sprites.insert({{X.object.position.x, X.object.position.y, X.object.position.z},X});
		        }
		    }
		}
	    }
	    else {
		for (auto& entry : sprites){
		    if (entry.second.object.name == name && entry.second.object.number == singleNumber){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()){
		            mesh X = it->second;

		            X.object.position += Vec3(x, y, 0);
		            sprites.erase(it);
		            sprites.insert({{X.object.position.x, X.object.position.y, X.object.position.z},X});
		        }
		    }
		}
	    }
	}
	else {
		const vector<int>& numbers = get<vector<int>>(number);
		for (int num : numbers) {
		    // Handle each number in the array
		    for (auto& entry : sprites) {
		        if (entry.second.object.name == name && entry.second.object.number == num) {
		            auto it = sprites.find(entry.first);
		            if (it != sprites.end()) {
		                mesh X = it->second;

		                X.object.position += Vec3{x, y, 0};  
		                sprites.erase(it);
		                sprites.insert({{X.object.position.x, X.object.position.y, X.object.position.z}, X});
		            }
		        }
		    }
		}
	}
}

void Silver::glideObjectRandom(const string& name, variant<int, vector<int>>& number, Vec3 position, float speed) {
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(0))); 
        seeded = true;
    }

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        
        if (singleNumber == origin) {
            return;
        } else {
            int randomX = rand() % WorldX;  
            int randomY = rand() % WorldY; 
            glideObjectXY(name, singleNumber, Vec2(randomX, randomY), speed);
        }
    } else {
    	const vector<int>& numbers = get<vector<int>>(number);
        int randomX = rand() % WorldX;  
        int randomY = rand() % WorldY; 
        glideObjectXY(name, numbers, Vec2(randomX, randomY), speed);
    }
}


void Silver::setObjectRandom(const string& name, const variant<int, vector<int>>& number) {
    // Seed the random number generator
    srand(static_cast<unsigned>(time(0)));

    // Lambda function to randomize object position
    auto randomizePosition = [this](auto& entry) {
        int randomX = rand() % WorldX;
        int randomY = rand() % WorldY;
        entry.object.position.x = randomX;
        entry.object.position.y = randomY;
    };

    // If the number is a single integer
    if (holds_alternative<int>(number)) {
        int num = get<int>(number);

        if (num == origin) {
            return;
        } 
        else if (num == all) {
            // Randomize position of all objects with the specified name
            for (auto& entry : sprites) {
                if (entry.second.object.name == name) {
                    auto it = sprites.find(entry.first);
                    if (it != sprites.end()) {
                        randomizePosition(it->second);
                        // Re-insert into the map based on new coordinates
                        sprites.erase(it);
                        sprites.insert({{it->second.object.position.x, it->second.object.position.y, it->second.object.position.z}, it->second});
                    }
                }
            }
        } 
        else {
            // Randomize position for objects with a matching name and number
            for (auto& entry : sprites) {
                if (entry.second.object.name == name && entry.second.object.number == num) {
                    auto it = sprites.find(entry.first);
                    if (it != sprites.end()) {
                        randomizePosition(it->second);
                        // Re-insert into the map based on new coordinates
                        sprites.erase(it);
                        sprites.insert({{it->second.object.position.x, it->second.object.position.y, it->second.object.position.z}, it->second});
                    }
                }
            }
        }
    } 
    // If the number is a vector of integers
    else if (holds_alternative<vector<int>>(number)) {
        vector<int> numList = get<vector<int>>(number);

        // Randomize positions for all objects with matching name and any number in the vector
        for (auto& entry : sprites) {
            if (entry.second.object.name == name && find(numList.begin(), numList.end(), entry.second.object.number) != numList.end()) {
                auto it = sprites.find(entry.first);
                if (it != sprites.end()) {
                    randomizePosition(it->second);
                    // Re-insert into the map based on new coordinates
                    sprites.erase(it);
                    sprites.insert({{it->second.object.position.x, it->second.object.position.y, it->second.object.position.z}, it->second});
                }
            }
        }
    }
}

struct termios orig_termios; // Store the original terminal settings


// Set terminal to raw mode
void Silver::setRawMode() {
struct termios raw;

// Get and save the original terminal attributes
tcgetattr(STDIN_FILENO, &orig_termios);

// Modify terminal attributes for raw mode
raw = orig_termios; // Start with the original settings
raw.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo

// Set the terminal attributes to raw mode
tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// Restore terminal to original settings
void Silver::resetMode() {
// Restore the original terminal attributes
tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}



void Silver::tRun(function<void()> func){
    thread t(func);
    t.detach();
}

void Silver::dRun(function<void()> func, float time){
    thread t([func, time]() {
        this_thread::sleep_for(chrono::milliseconds(static_cast<int>(time)));
        func();
    });
    t.detach();
}

vector<Vec3*> Silver::spriteLocation(const string& spriteName, int number) {
    static vector<Vec3*> locations; 
    locations.clear(); 

    for (auto& entry : sprites) {
        if (entry.second.object.name == spriteName && (number == all || entry.second.object.number == number)) {
            locations.push_back(&entry.second.object.position);
        }
    }

    // Return the first element if there's exactly one, or nullptr if empty
    return locations;
}

void Silver::setObjectPositionToSprite(const string& name, const variant<int, vector<int>>& number,
                                       const string& target, int targetNumber) {
    Vec3* targetLocation = spriteLocation(target, targetNumber)[0];

    if (targetLocation != nullptr) {
        // If number is an integer
        if (holds_alternative<int>(number)) {
            moveObjectPosition(name, get<int>(number), *targetLocation);
        }
        // If number is a vector of integers
        else if (holds_alternative<vector<int>>(number)) {
            vector<int> numList = get<vector<int>>(number);
            for (int num : numList) {
                moveObjectPosition(name, num, *targetLocation);  // Move each one to the same target location
            }
        }
    }
}

void Silver::glideObjectPositionToSprite(const string& name, const variant<int, vector<int>>& number,
                                         const string& target, int targetNumber, float speed) {
    Vec3* targetLocation = spriteLocation(target, targetNumber)[0];

    if (targetLocation != nullptr) {
        // If number is an integer
        if (holds_alternative<int>(number)) {
            glideObjectXY(name, get<int>(number), *targetLocation, speed);
        }
        // If number is a vector of integers
        else if (holds_alternative<vector<int>>(number)) {
            vector<int> numList = get<vector<int>>(number);
            for (int num : numList) {
                glideObjectXY(name, num, *targetLocation, speed);  // Glide each one to the same target location
            }
        }
    }
}

void Silver::bringToTop(const string& name, int number){
	if (number == origin){
		return; 
	    }
	    else if (number == all){
		for (auto& entry : sprites){
		    if (entry.second.object.name == name){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()) {
		            mesh X = it->second;

		            X.object.position.z = 0;
		            sprites.erase(it); 
		            sprites.insert({{X.object.position.x, X.object.position.y, X.object.position.z},X});
		        }
		    }
		}
	    }
	    else{
		for (auto& entry : sprites){
		    if (entry.second.object.name == name && entry.second.object.number == number){
		        auto it = sprites.find(entry.first);
		        if (it != sprites.end()){
		            mesh X = it->second;

		            X.object.position.z = 0;
		            sprites.erase(it);
		            sprites.insert({{X.object.position.x, X.object.position.y, X.object.position.z},X});
		        }
		    }
		}
	    }
}


