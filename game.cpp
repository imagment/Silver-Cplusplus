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
unordered_set<string> currentPressedKeys;
unordered_set<string> previousPressedKeys;
atomic<bool> keepListening(true);

mutex keyMutex;
multimap<tuple<int, int, int>, mesh> sprites;
multimap<tuple<int, int, int>, mesh> killedSprites;

map <string, objects> globalObjects;
vector<scene> scenes;

string out_of_bounds = "🟦";
string null_rep = "🧱";

int nextObjectPointer=0;
int WorldX = 20, WorldY = 20;

Silver silver;

void Silver::Camera::cell(int c){
	silver.camera.cellSize=c;
}

void Silver::Camera::configCameraException(string o, string n){
	null_rep=n;
	out_of_bounds=o;
}

vector<mesh> Silver::MeshAt(Vec3 pos, ...) {
    va_list args;
    va_start(args, pos);

    multimap<tuple<int, int, int>, mesh>* m = &sprites;
    if (void* temp = va_arg(args, void*)) {
        m = reinterpret_cast<multimap<tuple<int, int, int>, mesh>*>(temp);
    }

    va_end(args);

    auto range = m->equal_range(make_tuple(pos.x, pos.y, pos.z));

    vector<mesh> result;

    for (auto it = range.first; it != range.second; ++it) {
        result.push_back(it->second); 
    }

    return result; 
}

void Silver::PlaceMesh(mesh m, Vec3 pos) {
	sprites.insert({{pos.x,pos.y,pos.z},m});
} 

vector<mesh> Silver::GetMeshOf(string name, variant<int, vector<int>> number) {
    vector<mesh> result;

    for (auto& entry : sprites) {
        auto& obj = entry.second;

        if (name == pointer) { 
            if (holds_alternative<vector<int>>(number)) {
                auto& numbers = get<vector<int>>(number);

                if (find(numbers.begin(), numbers.end(), obj.objectPointer) != numbers.end()) {
                    result.push_back(obj);
                }
            } else { 
                if (obj.objectPointer == get<int>(number)) {
                    result.push_back(obj);
                }
            }
        } else { 
            if (holds_alternative<vector<int>>(number)) {
                auto& numbers = get<vector<int>>(number);

                if (obj.object.name == name && (find(numbers.begin(), numbers.end(), obj.object.number) != numbers.end() || get<int>(number) == all)) {
                    result.push_back(obj);
                }
            } else { 
                if (obj.object.name == name && (get<int>(number) == all || get<int>(number) == obj.object.number)) {
                    result.push_back(obj);
                }
            }
        }
    }

    return result; 
}

map <char, int> keys;

char Silver::Keyboard::getKey() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
    	if(ch>='a' && ch<='z') ch+='A'-'a';
    	keyBuffer=ch;
        return ch;  
    }
    keyBuffer='\0';
    return '\0'; 
}

bool Silver::Keyboard::isKey(char key) {
    if (keyBuffer==key) {
        return true;
    }
    return false;
}

bool Silver::Mouse::isMouse() {
    if (silver.keyboard.keyBuffer==mouseKey) {
        return true;
    }
    return false;
}

void Silver::gotoxy(int x, int y )
{
    printf("\033[%d;%dH", y+1, x+1);
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

void Silver::spray(int spawns, Vec3 center, int range, string c) {
    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < spawns; ++i) {
        int offsetX = rand() % (2 * range + 1) - range; 
        int offsetY = rand() % (2 * range + 1) - range;

        Vec3 position = {center.x + offsetX, center.y + offsetY, center.z};

        draw(position, c);  
    }
}
void Silver::sprayOval(int spawns, Vec3 center, Vec3 scale, string c) {
    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < spawns; ++i) {
        double angle = static_cast<double>(rand()) / RAND_MAX * 2 * M_PI;
        double distanceX = (static_cast<double>(rand()) / RAND_MAX) * scale.x;
        double distanceY = (static_cast<double>(rand()) / RAND_MAX) * scale.y;

        Vec3 position = {
            center.x + static_cast<int>(distanceX * cos(angle)),
            center.y + static_cast<int>(distanceY * sin(angle)),
            center.z
        };

        draw(position, c);  
    }
}

void Silver::sprayRectangle(int spawns, Vec3 center, Vec3 scale, string c) {
    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < spawns; ++i) {
        int offsetX = rand() % static_cast<int>(scale.x); 
        int offsetY = rand() % static_cast<int>(scale.y);

        Vec3 position = {center.x + offsetX, center.y + offsetY, center.z};

        draw(position, c);  
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
void Silver::sprayPlaceRectangle(string name, int number, int spawns, Vec3 center, Vec3 scale) {
    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < spawns; ++i) {
        int offsetX = rand() % static_cast<int>(scale.x); 
        int offsetY = rand() % static_cast<int>(scale.y);

        Vec3 position = {center.x + offsetX, center.y + offsetY, center.z};

        place(name, number, position);
    }
}
void Silver::sprayPlaceLine(string name, int number, int spawns, Vec3 start, Vec3 end) {
    srand(static_cast<unsigned int>(time(nullptr)));

    int dx = end.x - start.x;
    int dy = end.y - start.y;
    int dz = end.z - start.z;

    for (int i = 0; i < spawns; ++i) {
        double t = static_cast<double>(rand()) / RAND_MAX;  // Random value between 0 and 1
        
        Vec3 position = {
            static_cast<int>(start.x + t * dx),
            static_cast<int>(start.y + t * dy),
            static_cast<int>(start.z + t * dz)
        };

        place(name, number, position);
    }
}
void Silver::sprayPlaceOval(string name, int number, int spawns, Vec3 center, Vec3 scale) {
    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < spawns; ++i) {
        double angle = static_cast<double>(rand()) / RAND_MAX * 2 * M_PI;
        double distanceX = (static_cast<double>(rand()) / RAND_MAX) * scale.x;
        double distanceY = (static_cast<double>(rand()) / RAND_MAX) * scale.y;

        Vec3 position = {
            center.x + static_cast<int>(distanceX * cos(angle)),
            center.y + static_cast<int>(distanceY * sin(angle)),
            center.z
        };

        place(name, number, position);
    }
}
void Silver::sprayLine(int spawns, Vec3 start, Vec3 end, string c) {
    srand(static_cast<unsigned int>(time(nullptr)));

    int dx = end.x - start.x;
    int dy = end.y - start.y;
    int dz = end.z - start.z;

    for (int i = 0; i < spawns; ++i) {
        double t = static_cast<double>(rand()) / RAND_MAX;  // Random value between 0 and 1
        
        // Calculate a random point along the line
        Vec3 position = {
            static_cast<int>(start.x + t * dx),
            static_cast<int>(start.y + t * dy),
            static_cast<int>(start.z + t * dz)
        };

        draw(position, c);  
    }
}

void Silver::sprayPlace(string name, int number, int spawns, Vec3 center, int range) {
    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < spawns; ++i) {
        int offsetX = rand() % (2 * range + 1) - range; 
        int offsetY = rand() % (2 * range + 1) - range;

        Vec3 position = {center.x + offsetX, center.y + offsetY, center.z};

        place(name, number, position);
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
    int sx = (x1 < x2) ? 1 : -1; 
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        draw({x1, y1, start.z}, c); 
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

void Silver::drawOval(Vec3 center, Vec3 scale, string c) {
    for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
        for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
            int dx = x - center.x;
            int dy = y - center.y;

            if ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y)) {
                draw({x, y, center.z}, c);  
            }
        }
    }
}
void Silver::drawOvalHollow(Vec3 center, Vec3 scale, string c) {
    for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
        for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
            int dx = x - center.x;
            int dy = y - center.y;

            int first=((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y));
            scale.x--;
            scale.y--;
            int second=((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y));
            if (first && !second) {
                draw({x, y, center.z}, c);  
            }
            scale.x++;
            scale.y++;
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
void Silver::placeRectangle(string name, int number, Vec3 topLeft, int width, int height) {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            Vec3 pos = {topLeft.x + i, topLeft.y + j, topLeft.z};
            place(name, number, pos);  // Use place with name, number, and position
        }
    }
}

void Silver::placeRectangleHollow(string name, int number, Vec3 topLeft, int width, int height) {
    for (int i = 0; i < width; ++i) {
        place(name, number, {topLeft.x + i, topLeft.y, topLeft.z});                         
        place(name, number, {topLeft.x + i, topLeft.y + height - 1, topLeft.z});           
    }

    for (int j = 1; j < height - 1; ++j) {
        place(name, number, {topLeft.x, topLeft.y + j, topLeft.z});                         
        place(name, number, {topLeft.x + width - 1, topLeft.y + j, topLeft.z});            
    }
}

void Silver::placeCircle(string name, int number, Vec3 center, int radius) {
    for (int y = center.y - radius; y <= center.y + radius; ++y) {
        for (int x = center.x - radius; x <= center.x + radius; ++x) {
            int dx = x - center.x;
            int dy = y - center.y;

            if (dx * dx + dy * dy <= radius * radius) {
                place(name, number, {x, y, center.z});  // Use place with name, number, and position
            }
        }
    }
}

void Silver::placeLine(string name, int number, Vec3 start, Vec3 end) {
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
        place(name, number, {x1, y1, start.z});  // Use place with name, number, and position
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

void Silver::placeOval(string name, int number, Vec3 center, Vec3 scale) {
    for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
        for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
            int dx = x - center.x;
            int dy = y - center.y;

            if ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y)) {
                place(name, number, {x, y, center.z});  // Use place with name, number, and position
            }
        }
    }
}

void Silver::placeOvalHollow(string name, int number, Vec3 center, Vec3 scale) {
    for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
        for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
            int dx = x - center.x;
            int dy = y - center.y;

            int first = ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y));
            scale.x--;
            scale.y--;
            int second = ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y));
            if (first && !second) {
                place(name, number, {x, y, center.z});  // Use place with name, number, and position
            }
            scale.x++;
            scale.y++;
        }
    }
}

void Silver::placeCircleHollow(string name, int number, Vec3 center, int radius) {
    for (int y = center.y - radius; y <= center.y + radius; ++y) {
        for (int x = center.x - radius; x <= center.x + radius; ++x) {
            int dx = x - center.x;
            int dy = y - center.y;
            int distanceSquared = dx * dx + dy * dy;

            if (distanceSquared >= (radius - 1) * (radius - 1) && distanceSquared <= radius * radius) {
                place(name, number, {x, y, center.z});  // Use place with name, number, and position
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
    isFirst=false;
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

vector <int> Silver::seek(string objectName) {
	vector <int> numbers;
	for(auto entry : sprites) {
		if(entry.second.object.name==objectName){
			numbers.push_back(entry.second.object.number);
		}
	}
	return numbers;
}

vector<int> Silver::seekUnique(string objectName) {
    set<int> uniqueNumbers;
    for(auto entry : sprites) {
        if(entry.second.object.name == objectName) {
            uniqueNumbers.insert(entry.second.object.number);
        }
    }
    return vector<int>(uniqueNumbers.begin(), uniqueNumbers.end());
}

void Silver::destroy(const string& objectName, const variant<int, vector<int>>& number) {
    vector<tuple<int, int, int>> keysToRemove;
    vector<string> globalKeysToRemove;

    if (objectName == pointer) {

        if (holds_alternative<int>(number)) {
            int numValue = get<int>(number);
            for (const auto& entry : sprites) {
                if (entry.second.objectPointer == numValue) {
                    keysToRemove.push_back(entry.first);
                }
            }
        } 

        else if (holds_alternative<vector<int>>(number)) {
            vector<int> numValues = get<vector<int>>(number);
            for (const auto& entry : sprites) {
                if (find(numValues.begin(), numValues.end(), entry.second.objectPointer) != numValues.end()) {
                    keysToRemove.push_back(entry.first);
                }
            }
        }
    } else {
        if (holds_alternative<int>(number)) {
            int numValue = get<int>(number);
            if (numValue == origin) {

                for (const auto& entry : globalObjects) {
                    if (entry.second.name == objectName) {
                        globalKeysToRemove.push_back(entry.first); 
                    }
                }
            } else {

                for (const auto& entry : sprites) {
                    if (entry.second.object.name == objectName &&
                        (entry.second.object.number == all || entry.second.object.number == numValue)) {
                        keysToRemove.push_back(entry.first); 
                    }
                }
            }
        } else if (holds_alternative<vector<int>>(number)) {
            vector<int> numValues = get<vector<int>>(number);

            for (const auto& entry : sprites) {
                if (entry.second.object.name == objectName &&
                    (entry.second.object.number == all ||
                     find(numValues.begin(), numValues.end(), entry.second.object.number) != numValues.end())) {
                    keysToRemove.push_back(entry.first); 
                }
            }
        }
    }

    for (const auto& key : keysToRemove) {
        sprites.erase(key);
    }

    for (const auto& key : globalKeysToRemove) {
        globalObjects.erase(key);
    }
}

void Silver::kill(const string& objectName, const variant<int, vector<int>>& number) {
    vector<tuple<int, int, int>> keysToKill;
    set<int> numberSet; 

    if (holds_alternative<int>(number)) {
        numberSet.insert(get<int>(number));
    } else {
        numberSet = set<int>(get<vector<int>>(number).begin(),
                             get<vector<int>>(number).end());
    }

    if (objectName == pointer) {
        for (const auto& entry : sprites) {

            if (numberSet.count(entry.second.objectPointer)) {
                keysToKill.push_back(entry.first); 
            }
        }
    } else {

        for (const auto& entry : sprites) {
            if (entry.second.object.name == objectName &&
                (numberSet.count(all) || numberSet.count(entry.second.object.number))) {
                keysToKill.push_back(entry.first); 
            }
        }
    }

    for (const auto& key : keysToKill) {
        killedSprites.insert({key, sprites.find(key)->second});  
        sprites.erase(key);  
    }
}

void Silver::revive(const string& objectName, const variant<int, vector<int>>& number) {
    vector<tuple<int, int, int>> keysToRevive;
    set<int> numberSet; 

    if (holds_alternative<int>(number)) {
        numberSet.insert(get<int>(number));
    } else {
        numberSet = set<int>(get<vector<int>>(number).begin(), get<vector<int>>(number).end());
    }

    if (objectName == pointer) {
        for (const auto& entry : killedSprites) {
            if (numberSet.count(entry.second.objectPointer)) {
                keysToRevive.push_back(entry.first);
            }
        }
    } else {
        for (const auto& entry : killedSprites) {
            if (entry.second.object.name == objectName &&
                (numberSet.count(all) || numberSet.count(entry.second.object.number))) {
                keysToRevive.push_back(entry.first);
            }
        }
    }

    for (const auto& key : keysToRevive) {
        sprites.insert({key, killedSprites.find(key)->second});
        killedSprites.erase(key);
    }
}

void Silver::Camera::printCam() {
    if (isFirst) {
        silver.setRawMode();
        silver.clear();
    }

    vector<vector<string>> buffer(silver.camera.CameraScale.y, vector<string>(silver.camera.CameraScale.x, null_rep));

    float radians = silver.camera.cameraRotation * (M_PI / 180.0);
    float cosTheta = cos(radians);
    float sinTheta = sin(radians);

    int centerX = silver.camera.CameraScale.x / 2;
    int centerY = silver.camera.CameraScale.y / 2;

    for (int j = 0; j < silver.camera.CameraScale.y; ++j) {
        for (int i = 0; i < silver.camera.CameraScale.x; ++i) {
            int dx = i - centerX;
            int dy = j - centerY;

            int rotatedX = round(cosTheta * dx + sinTheta * dy + silver.camera.CameraPos.x);
            int rotatedY = round(-sinTheta * dx + cosTheta * dy + silver.camera.CameraPos.y);

            if (silver.camera.isFlippedHorizonal == -1) {
                rotatedX = WorldX - rotatedX - 1;
            }
            if (silver.camera.isFlippedVertical == -1) {
                rotatedY = WorldY - rotatedY - 1;
            }

            string currentShape = (rotatedX < 0 || rotatedY < 0 || rotatedX >= WorldX || rotatedY >= WorldY) 
                                    ? out_of_bounds 
                                    : null_rep;

            for (int z = silver.camera.CameraPos.z + silver.camera.CameraScale.z / 2 - 1; z >= silver.camera.CameraPos.z + (-silver.camera.CameraScale.z + silver.camera.CameraScale.z / 2); --z) {
                auto it = sprites.find({rotatedX, rotatedY, z});
                if (it != sprites.end() && it->second.transparency == 0) {
                    currentShape = it->second.shape;
                    break;
                }
            }

            buffer[j][i] = currentShape;
        }
    }

    if (!gridMode) {
        int mouseX = silver.mouse.cursorPositionX; 
        int mouseY = silver.mouse.cursorPositionY; 

        for (int j = 0; j < silver.camera.CameraScale.y; ++j) {
            string line;
            for (int i = 0; i < silver.camera.CameraScale.x; ++i) {
                string cellContent = (i == mouseX && j == mouseY) ? "🖱️" : buffer[j][i];

                line += cellContent;
            }

            silver.gotoxy(0, j); 
            cout << line << flush;
        }
    } else { 
    int mouseX = silver.mouse.cursorPositionX; 
    int mouseY = silver.mouse.cursorPositionY; 

    for (int j = 0; j < silver.camera.CameraScale.y; ++j) {
        for (int i = 0; i < silver.camera.CameraScale.x; ++i) {
            auto currentFrame = make_tuple(i, j);
            string cellContent;

            if (i == mouseX && j == mouseY) {
                cellContent = "🖱️"; 
            } else {
                cellContent = buffer[j][i];
            }

            if (cellContent.size() > silver.camera.cellSize * 4) {
                cellContent = cellContent.substr(0, silver.camera.cellSize);
            }

            if (fillMode && cellContent.size() < silver.camera.cellSize) {
                cellContent.append(silver.camera.cellSize - cellContent.size(), ' '); 
            }

            if (isFirst || silver.camera.lastFrame[currentFrame] != cellContent) {
                silver.gotoxy(i * silver.camera.cellSize, j);
                cout << cellContent << flush; 
                silver.camera.lastFrame[currentFrame] = cellContent; 
            }
        }
    }
}

    isFirst = false;

    bool sign = false;
    string signMessage, signIcon, objectName;

    for (int j = 0; j < silver.camera.CameraScale.y; ++j) {
        for (int i = 0; i < silver.camera.CameraScale.x; ++i) {
            int dx = i - centerX;
            int dy = j - centerY;

            int rotatedX = round(cosTheta * dx + sinTheta * dy + silver.camera.CameraPos.x);
            int rotatedY = round(-sinTheta * dx + cosTheta * dy + silver.camera.CameraPos.y);

            if (silver.camera.isFlippedHorizonal == -1) {
                rotatedX = WorldX - rotatedX - 1;
            }
            if (silver.camera.isFlippedVertical == -1) {
                rotatedY = WorldY - rotatedY - 1;
            }

            if (rotatedX >= 0 && rotatedY >= 0 && rotatedX < WorldX && rotatedY < WorldY) {
                for (int z = silver.camera.CameraPos.z + silver.camera.CameraScale.z / 2 - 1; z >= silver.camera.CameraPos.z + (-silver.camera.CameraScale.z + silver.camera.CameraScale.z / 2); --z) {
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

    if (sign) {
        cout << "\n\n*************************************\n"
             << signIcon << string(silver.camera.cellSize, ' ') << objectName << endl
             << signMessage << endl;
    }
}

Vec2 Silver::Camera::getScreenPosition(Vec3 pos) {
    int worldX=pos.x;
    int worldY=pos.y;
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

    return {screenX, screenY};
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

atomic<bool> VMouse = false; 
mutex mouseMutex; 

void VMouseOn(int l, int r, int u, int d, int c) {
    while (VMouse.load()) {
        char det = silver.keyboard.getKey();

        {

            lock_guard<mutex> lock(mouseMutex);
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
    silver.gotoxy(0,1);
    cout << "#" << flush;
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

int Silver::place(string objectName, int number, Vec3 position) {
    if (number < 0) {
        number = 0;
    }
    if (globalObjects.count(objectName) == 0){
        return -1;
    }

    auto entry = globalObjects.find(objectName);
    if (entry != globalObjects.end()) {
        mesh X;
        X.object.position = position;
        X.object.number = number;
        X.object.name = objectName;
        X.shape = entry->second.shape;
        X.transparency = entry->second.transparency;
        X.anim = entry->second.anim;
        X.Components = entry->second.Components;

	X.objectPointer = nextObjectPointer++;
        sprites.insert({{position.x, position.y, position.z},X});
    }
    return nextObjectPointer-1;
}

int Silver::put(string objectName, Vec3 position) {
    auto entry = globalObjects.find(objectName);
    if (entry != globalObjects.end()) {
        mesh X;
        X.object.position = position;
        X.object.number = 0;
        X.object.name = objectName;
        X.shape = entry->second.shape;
        X.transparency = entry->second.transparency;
        X.anim = entry->second.anim;
        X.objectPointer = nextObjectPointer++;

        X.Components = entry->second.Components;

        sprites.insert({{position.x, position.y, position.z},X});
    }
    return nextObjectPointer-1;
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
thread videoThread;
mutex camMutex;

void Silver::Camera::startVideo(int FPS) {

    lock_guard<mutex> lock(camMutex);  
    if (isRunningCam) return;  

    isRunningCam = true;
    videoThread = thread([FPS] { 
        while (isRunningCam) {
            silver.camera.printCam();
            this_thread::sleep_for(chrono::milliseconds(static_cast<int>(1000.0f / FPS)));
        }
    });
}

void Silver::Camera::endVideo() {
    {
        lock_guard<mutex> lock(camMutex);  
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
    else if (component == "sign_reader") { 
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

    auto range = sprites.equal_range({pos.x, pos.y, pos.z});
    for (auto it = range.first; it != range.second; ++it) {
        result.push_back(it->second.object.name); 
    }

    return result;
}
vector<string> Silver::spriteAt2(Vec3 pos) {
    vector<string> names;

    if (pos.x >= 0 && pos.y >= 0 && pos.x < WorldX && pos.y < WorldY) {

        for (auto& entry : sprites) {
            if (entry.second.object.position.x == pos.x && entry.second.object.position.y == pos.y) {

                names.push_back(entry.second.object.name);
            }
        }
    }

    return names;
}

void Silver::setObjectXY(const string& name, const variant<int, vector<int>>& number, Vec3 pos) {
    int x = pos.x;
    int y = pos.y;

    vector<tuple<int, int, int>> keysToUpdate;

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        if (singleNumber == origin) {
            return;
        } else if (singleNumber == all) {
            for (auto& entry : sprites) {
                if (name == pointer) {
                    if (entry.second.objectPointer) {
                        keysToUpdate.push_back(entry.first);
                    }
                } else if (name == entry.second.object.name) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        } else {
            for (auto& entry : sprites) {
                if (name == pointer) {
                    if (entry.second.objectPointer == singleNumber) {
                        keysToUpdate.push_back(entry.first);
                    }
                } else if (entry.second.object.name == name && entry.second.object.number == singleNumber) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        }
    } else {
        const vector<int>& numbers = get<vector<int>>(number);
        unordered_set<int> numberSet(numbers.begin(), numbers.end());

        for (auto& entry : sprites) {
            if (name == pointer) {
                if (numberSet.count(entry.second.objectPointer)) {
                    keysToUpdate.push_back(entry.first);
                }
            } else if (entry.second.object.name == name && numberSet.count(entry.second.object.number)) {
                keysToUpdate.push_back(entry.first);
            }
        }
    }

    for (auto& key : keysToUpdate) {
        auto it = sprites.find(key);
        if (it != sprites.end()) {
            mesh& X = it->second;
            int z = X.object.position.z;

            X.object.position = {x, y, z};

            sprites.erase(it);
            sprites.insert({{x, y, z}, X});
        }
    }
}

void Silver::setObjectPosition(const string& name, const variant<int, vector<int>>& number, Vec3 pos) {
    int x = pos.x;
    int y = pos.y;
    int z = pos.z;

    vector<tuple<int, int, int>> keysToUpdate; 

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        if (singleNumber == origin) {
            return;
        }

        if (singleNumber == all) {
            for (auto& entry : sprites) {
                if (entry.second.object.name == name) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        } else {
            for (auto& entry : sprites) {
                if (entry.second.object.name == name && entry.second.object.number == singleNumber) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        }
    } else {
        const vector<int>& numbers = get<vector<int>>(number);
        unordered_set<int> numberSet(numbers.begin(), numbers.end());

        for (auto& entry : sprites) {
            if (entry.second.object.name == name && numberSet.count(entry.second.object.number)) {
                keysToUpdate.push_back(entry.first);
            }
        }
    }

    for (const auto& key : keysToUpdate) {
        auto it = sprites.find(key);
        if (it != sprites.end()) {
            mesh& X = it->second;
	    X.object.position = {x, y, z};

	    sprites.erase(it); 
	    sprites.insert({{x, y, z}, X}); 

        }
    }
}

void Silver::moveObjectXY(const string& name, const variant<int, vector<int>>& number, Vec3 pos) {
    int dx = pos.x;
    int dy = pos.y;

    vector<tuple<int, int, int>> keysToUpdate; 

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        if (singleNumber == origin) {
            return;
        }

        if (singleNumber == all) {
            for (auto& entry : sprites) {
                if (entry.second.object.name == name) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        } else {
            for (auto& entry : sprites) {
                if (entry.second.object.name == name && entry.second.object.number == singleNumber) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        }
    } else {
        const vector<int>& numbers = get<vector<int>>(number);
        unordered_set<int> numberSet(numbers.begin(), numbers.end());

        for (auto& entry : sprites) {
            if (entry.second.object.name == name && numberSet.count(entry.second.object.number)) {
                keysToUpdate.push_back(entry.first);
            }
        }
    }

    for (const auto& key : keysToUpdate) {
	    auto it = sprites.find(key);
	    if (it != sprites.end()) {
		mesh& X = it->second;
		X.object.position += Vec3(dx, dy, 0);
		int x=X.object.position.x+dx;
		int y=X.object.position.x+dy;
		int z=X.object.position.x;
		sprites.erase(it);
		sprites.insert({{x,y,z}, X});
	    }
    }

}

void Silver::moveObjectPosition(const string& name, const variant<int, vector<int>>& number, Vec3 pos) {
    int dx = pos.x;
    int dy = pos.y;
    int dz = pos.z;

    vector<tuple<int, int, int>> keysToUpdate; 

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        if (singleNumber == origin) {
            return;
        }

        if (singleNumber == all) {
            for (auto& entry : sprites) {
                if (entry.second.object.name == name) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        } else {
            for (auto& entry : sprites) {
                if (entry.second.object.name == name && entry.second.object.number == singleNumber) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        }
    } else {
        const vector<int>& numbers = get<vector<int>>(number);
        unordered_set<int> numberSet(numbers.begin(), numbers.end());

        for (auto& entry : sprites) {
            if (entry.second.object.name == name && numberSet.count(entry.second.object.number)) {
                keysToUpdate.push_back(entry.first);
            }
        }
    }

    for (const auto& key : keysToUpdate) {
	    auto it = sprites.find(key);
	    if (it != sprites.end()) {
		mesh& X = it->second;
		X.object.position += Vec3(dx, dy, dz);
		int x=X.object.position.x+dx;
		int y=X.object.position.x+dy;
		int z=X.object.position.x+dz;
		sprites.erase(it);
		sprites.insert({{x,y,z}, X});
	    }
    }

}

void Silver::moveObjectX(const string& name, const variant<int, vector<int>>& number, int x_offset) {
    int dx = x_offset;
    vector<tuple<int, int, int>> keysToUpdate;

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        if (singleNumber == origin) {
            return;
        } else if (singleNumber == all) {
            for (const auto& entry : sprites) {
                if (name == pointer) {
                    if (entry.second.objectPointer) {
                        keysToUpdate.push_back(entry.first);
                    }
                } else if (entry.second.object.name == name) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        } else {
            for (const auto& entry : sprites) {
                if (name == pointer) {
                    if (entry.second.objectPointer == singleNumber) {
                        keysToUpdate.push_back(entry.first);
                    }
                } else if (entry.second.object.name == name && entry.second.object.number == singleNumber) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        }
    } else {
        const vector<int>& numbers = get<vector<int>>(number);
        for (const auto& entry : sprites) {
            if (name == pointer) {
                if (find(numbers.begin(), numbers.end(), entry.second.objectPointer) != numbers.end()) {
                    keysToUpdate.push_back(entry.first);
                }
            } else if (entry.second.object.name == name &&
                       find(numbers.begin(), numbers.end(), entry.second.object.number) != numbers.end()) {
                keysToUpdate.push_back(entry.first);
            }
        }
    }

    for (const auto& key : keysToUpdate) {
        auto it = sprites.find(key);
        if (it != sprites.end()) {
            mesh X = it->second;
            X.object.position += Vec3(dx, 0, 0);
            int x = X.object.position.x;
            int y = X.object.position.y;
            int z = X.object.position.z;
            sprites.erase(it);
            sprites.insert({{x, y, z}, X});
        }
    }
}

void Silver::moveObjectY(const string& name, const variant<int, vector<int>>& number, int y_offset) {
    int dy = y_offset;
    vector<tuple<int, int, int>> keysToUpdate;

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        if (singleNumber == origin) {
            return;
        } else if (singleNumber == all) {
            for (const auto& entry : sprites) {
                if (name == pointer) {
                    if (entry.second.objectPointer) {
                        keysToUpdate.push_back(entry.first);
                    }
                } else if (entry.second.object.name == name) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        } else {
            for (const auto& entry : sprites) {
                if (name == pointer) {
                    if (entry.second.objectPointer == singleNumber) {
                        keysToUpdate.push_back(entry.first);
                    }
                } else if (entry.second.object.name == name && entry.second.object.number == singleNumber) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        }
    } else {
        const vector<int>& numbers = get<vector<int>>(number);
        for (const auto& entry : sprites) {
            if (name == pointer) {
                if (find(numbers.begin(), numbers.end(), entry.second.objectPointer) != numbers.end()) {
                    keysToUpdate.push_back(entry.first);
                }
            } else if (entry.second.object.name == name &&
                       find(numbers.begin(), numbers.end(), entry.second.object.number) != numbers.end()) {
                keysToUpdate.push_back(entry.first);
            }
        }
    }

    for (const auto& key : keysToUpdate) {
        auto it = sprites.find(key);
        if (it != sprites.end()) {
            mesh X = it->second;
            X.object.position += Vec3(0, dy, 0);
            int x = X.object.position.x;
            int y = X.object.position.y;
            int z = X.object.position.z;
            sprites.erase(it);
            sprites.insert({{x, y, z}, X});
        }
    }
}
void Silver::setObjectX(const string& name, const variant<int, vector<int>>& number, int x) {
    vector<tuple<int, int, int>> keysToUpdate;

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        if (singleNumber == origin) {
            return;
        } else if (singleNumber == all) {
            for (const auto& entry : sprites) {
                if (name == pointer) {
                    if (entry.second.objectPointer) {
                        keysToUpdate.push_back(entry.first);
                    }
                } else if (entry.second.object.name == name) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        } else {
            for (const auto& entry : sprites) {
                if (name == pointer) {
                    if (entry.second.objectPointer == singleNumber) {
                        keysToUpdate.push_back(entry.first);
                    }
                } else if (entry.second.object.name == name && entry.second.object.number == singleNumber) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        }
    } else {
        const vector<int>& numbers = get<vector<int>>(number);
        for (const auto& entry : sprites) {
            if (name == pointer) {
                if (find(numbers.begin(), numbers.end(), entry.second.objectPointer) != numbers.end()) {
                    keysToUpdate.push_back(entry.first);
                }
            } else if (entry.second.object.name == name &&
                       find(numbers.begin(), numbers.end(), entry.second.object.number) != numbers.end()) {
                keysToUpdate.push_back(entry.first);
            }
        }
    }

    for (const auto& key : keysToUpdate) {
        auto it = sprites.find(key);
        if (it != sprites.end()) {
            mesh X = it->second;
            int y = X.object.position.y;
            int z = X.object.position.z;
            X.object.position = Vec3(x, y, z);
            sprites.erase(it);
            sprites.insert({{x, y, z}, X});
        }
    }
}
void Silver::glideObjectX(string name, const variant<int, vector<int>>& number, int x_offset, float speed) {
    int dir = (x_offset > 0) ? 1 : -1;

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        if (singleNumber == origin) return;

        vector<string> keysToUpdate;

        for (const auto& entry : sprites) {
            if ((name == pointer && entry.second.objectPointer == singleNumber) || 
                (entry.second.object.name == name && (singleNumber == all || entry.second.object.number == singleNumber))) {
                keysToUpdate.push_back(entry.second.object.name);
            }
        }

        for (int i = 0; i < abs(x_offset); i++) {
            silver.wait(speed);

            for (const auto& key : keysToUpdate) {
                silver.moveObjectX(key, singleNumber, dir);
            }
        }

    } else {
        const vector<int>& numbers = get<vector<int>>(number);
        vector<pair<string, int>> objectsToMove;

        for (const auto& entry : sprites) {
            if ((name == pointer && find(numbers.begin(), numbers.end(), entry.second.objectPointer) != numbers.end()) || 
                (entry.second.object.name == name && find(numbers.begin(), numbers.end(), entry.second.object.number) != numbers.end())) {
                objectsToMove.push_back(make_pair(entry.second.object.name, entry.second.object.number));
            }
        }

        for (int i = 0; i < abs(x_offset); i++) {
            silver.wait(speed);

            for (const auto& [key, num] : objectsToMove) {
                silver.moveObjectX(key, num, dir);
            }
        }
    }
}

void Silver::glideObjectY(string name, const variant<int, vector<int>>& number, int y_offset, float speed) {
    int dir = (y_offset > 0) ? 1 : -1;

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        if (singleNumber == origin) return;

        vector<string> keysToUpdate;

        for (const auto& entry : sprites) {
            if ((name == pointer && entry.second.objectPointer == singleNumber) || 
                (entry.second.object.name == name && (singleNumber == all || entry.second.object.number == singleNumber))) {
                keysToUpdate.push_back(entry.second.object.name);
            }
        }

        for (int i = 0; i < abs(y_offset); i++) {
            silver.wait(speed);

            for (const auto& key : keysToUpdate) {
                silver.moveObjectY(key, singleNumber, dir);
            }
        }
    } else {
        const vector<int>& numbers = get<vector<int>>(number);
        vector<pair<string, int>> objectsToMove;

        for (const auto& entry : sprites) {
            if ((name == pointer && find(numbers.begin(), numbers.end(), entry.second.objectPointer) != numbers.end()) || 
                (entry.second.object.name == name && find(numbers.begin(), numbers.end(), entry.second.object.number) != numbers.end())) {
                objectsToMove.push_back(make_pair(entry.second.object.name, entry.second.object.number));
            }
        }

        for (int i = 0; i < abs(y_offset); i++) {
            silver.wait(speed);

            for (const auto& [key, num] : objectsToMove) {
                silver.moveObjectY(key, num, dir);
            }
        }
    }
}
void Silver::setObjectY(const string& name, const variant<int, vector<int>>& number, int y) {
    vector<tuple<int, int, int>> keysToUpdate;

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        if (singleNumber == origin) {
            return;
        }

        if (singleNumber == all) {
            for (const auto& entry : sprites) {
                if ((name == pointer && entry.second.objectPointer) || entry.second.object.name == name) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        } else {
            for (const auto& entry : sprites) {
                if ((name == pointer && entry.second.objectPointer == singleNumber) || 
                    (entry.second.object.name == name && entry.second.object.number == singleNumber)) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        }
    } else {
        const vector<int>& numbers = get<vector<int>>(number);
        for (const auto& entry : sprites) {
            if ((name == pointer && find(numbers.begin(), numbers.end(), entry.second.objectPointer) != numbers.end()) || 
                (entry.second.object.name == name && find(numbers.begin(), numbers.end(), entry.second.object.number) != numbers.end())) {
                keysToUpdate.push_back(entry.first);
            }
        }
    }

    for (const auto& key : keysToUpdate) {
        auto it = sprites.find(key);
        if (it != sprites.end()) {
            mesh& currentMesh = it->second;
            int x = currentMesh.object.position.x;
            int z = currentMesh.object.position.z;

            currentMesh.object.position.y = y;

            auto value = currentMesh;
            sprites.erase(it);
            sprites.insert({{x, y, z}, value});
        }
    }
}
void Silver::glideObjectXY(string name, const variant<vector<int>, int>& number, Vec3 target_pos, float speed, ...) {
    va_list args;
    va_start(args, speed);
    bool setPosition = false;

    if (va_arg(args, int)) {
        setPosition = true;
    }

    va_end(args);

    vector<tuple<int, int, int>> keysToUpdate;

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        if (singleNumber == origin) {
            return;
        } else if (singleNumber == all) {
            for (const auto& entry : sprites) {
                if ((name == pointer && entry.second.objectPointer==singleNumber) || entry.second.object.name == name) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        } else {
            for (const auto& entry : sprites) {
                if ((name == pointer && entry.second.objectPointer == singleNumber) || 
                    (entry.second.object.name == name && entry.second.object.number == singleNumber)) {
                    keysToUpdate.push_back(entry.first);
                }
            }
        }
    } else {
        const vector<int>& numbers = get<vector<int>>(number);
        for (const auto& entry : sprites) {
            if ((name == pointer && find(numbers.begin(), numbers.end(), entry.second.objectPointer) != numbers.end()) || 
                (entry.second.object.name == name && find(numbers.begin(), numbers.end(), entry.second.object.number) != numbers.end())) {
                keysToUpdate.push_back(entry.first);
            }
        }
    }

    for (const auto& key : keysToUpdate) {
        auto it = sprites.find(key);
        if (it != sprites.end()) {
            mesh& X = it->second;

            int start_x = X.object.position.x;
            int start_y = X.object.position.y;

            int target_x = setPosition ? target_pos.x : start_x + target_pos.x;
            int target_y = setPosition ? target_pos.y : start_y + target_pos.y;

            int deltaX = abs(target_x - start_x);
            int deltaY = abs(target_y - start_y);

            int signX = (start_x < target_x) ? 1 : -1;
            int signY = (start_y < target_y) ? 1 : -1;

            int err = deltaX - deltaY;

            while (start_x != target_x || start_y != target_y) {

                int e2 = 2 * err;
                if (e2 > -deltaY) {
                    err -= deltaY;
                    start_x += signX;
                }
                if (e2 < deltaX) {
                    err += deltaX;
                    start_y += signY;
                }
		silver.setObjectXY(name, X.object.number, Vec2(start_x,start_y));
                silver.wait(speed);
            }

            silver.setObjectXY(name, X.object.number, Vec2(target_x,target_y));
        }
    }
}

void Silver::glideObjectRandom(const string& name, const variant<int, vector<int>>& number, float speed) {
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(0)));
        seeded = true;
    }

    int randomX = rand() % WorldX;  
    int randomY = rand() % WorldY;

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        if (singleNumber == origin) return;

        if (name == pointer) {
            glideObjectXY(pointer, singleNumber, Vec2(randomX, randomY), speed);
        } else {
            glideObjectXY(name, singleNumber, Vec2(randomX, randomY), speed);
        }
    } else {
        const vector<int>& numbers = get<vector<int>>(number);

        if (name == pointer) {
            glideObjectXY(pointer, numbers, Vec2(randomX, randomY), speed);
        } else {
            glideObjectXY(name, numbers, Vec2(randomX, randomY), speed);
        }
    }
}

void Silver::setObjectRandom(const string& name, const variant<int, vector<int>>& number) {
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(0)));
        seeded = true;
    }

    int randomX = rand() % WorldX;  
    int randomY = rand() % WorldY;

    if (holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        if (singleNumber == origin) return;

        if (name == pointer) {
            setObjectXY(pointer, singleNumber, Vec2(randomX, randomY));
        } else {
            setObjectXY(name, singleNumber, Vec2(randomX, randomY));
        }
    } else {
        const vector<int>& numbers = get<vector<int>>(number);

        if (name == pointer) {
            setObjectXY(pointer, numbers, Vec2(randomX, randomY));
        } else {
            setObjectXY(name, numbers, Vec2(randomX, randomY));
        }
    }
}

struct termios orig_termios; 

void Silver::setRawMode() {
	struct termios raw;

	tcgetattr(STDIN_FILENO, &orig_termios);

	raw = orig_termios; 
	raw.c_lflag &= ~(ICANON | ECHO); 

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void Silver::resetMode() {

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void Silver::Threading::tRun(function<void()> func) {
    thread t(func);
    t.detach();
}

void Silver::Threading::dRun(function<void()> func, float time) {
    thread t([func, time]() {
        this_thread::sleep_for(chrono::milliseconds(static_cast<int>(time)));
        func();
    });
    t.detach();
}

void Silver::Threading::createThread(const string& name, function<void()> func) {
    lock_guard<mutex> lock(thread_mutex);

    if (threads.find(name) != threads.end()) return; 

    auto threadInfo = make_shared<ThreadInfo>();

    threadInfo->t = thread([this, name, func, threadInfo]() {
        unique_lock<mutex> lock(threadInfo->mtx);
        while (!threadInfo->stop) {

            if (threadInfo->paused) {
                threadInfo->cv.wait(lock); 
            } else {
                func(); 
                break; 
            }
        }
    });

    threads[name] = threadInfo; 
}

void Silver::Threading::destroyThread(const string& name) {
    lock_guard<mutex> lock(thread_mutex);
    if (threads.find(name) != threads.end()) {
        {
            lock_guard<mutex> th_lock(threads[name]->mtx);
	    threads[name]->stop = true;
	    threads[name]->cv.notify_all();

        }
        if (threads[name]->t.joinable()) threads[name]->t.join();
        threads.erase(name);
    }
}

void Silver::Threading::stopAllThreads() {
    lock_guard<mutex> lock(thread_mutex);
    for (auto& pair : threads) {
        {
            lock_guard<mutex> th_lock(pair.second->mtx);
	    pair.second->stop = true;
	    pair.second->cv.notify_all();

        }
        if (pair.second->t.joinable()) pair.second->t.join();
    }
    threads.clear();
}

void Silver::Threading::pauseThread(const string& name) {
    lock_guard<mutex> lock(thread_mutex);
    if (threads.find(name) != threads.end() && !threads[name]->paused) {
        lock_guard<mutex> th_lock(threads[name]->mtx);
	threads[name]->paused = true;

    }
}

void Silver::Threading::resumeThread(const string& name) {
    lock_guard<mutex> lock(thread_mutex);
    if (threads.find(name) != threads.end() && threads[name]->paused) {

        lock_guard<mutex> th_lock(threads[name]->mtx);
	threads[name]->paused = false;
	threads[name]->cv.notify_all();

    }
}

vector<Vec3> Silver::getOvalPoints(Vec3 center, Vec3 scale) {
    vector<Vec3> points;
    for (int y = center.y - scale.y; y <= center.y + scale.y; ++y) {
        for (int x = center.x - scale.x; x <= center.x + scale.x; ++x) {
            int dx = x - center.x;
            int dy = y - center.y;
            if ((dx * dx) * (scale.y * scale.y) + (dy * dy) * (scale.x * scale.x) <= (scale.x * scale.x * scale.y * scale.y)) {
                points.push_back({x, y, center.z});
            }
        }
    }
    return points;
}

vector<Vec3> Silver::getOvalHollowPoints(Vec3 center, Vec3 scale) {
    vector<Vec3> points;
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
                points.push_back({x, y, center.z});
            }
        }
    }
    return points;
}

vector<Vec3> Silver::getLinePoints(Vec3 start, Vec3 end) {
    vector<Vec3> points;

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

vector<Vec3> Silver::getRectanglePoints(Vec3 topLeft, int width, int height) {
    vector<Vec3> points;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            points.push_back({topLeft.x + i, topLeft.y + j, topLeft.z});
        }
    }
    return points;
}
vector<Vec3> Silver::getRectangleHollowPoints(Vec3 topLeft, int width, int height) {
    vector<Vec3> points;

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

vector<Vec3> Silver::getLocation(const string& name, int number) {
    vector<Vec3> locations; 

    for (auto& entry : sprites) {
        if(name==pointer) {
            if(entry.second.objectPointer==number) {
                locations.push_back(entry.second.object.position); 
            }
            continue;
        }
        if (entry.second.object.name == name && (number == all || entry.second.object.number == number)) {
            locations.push_back(entry.second.object.position); 
        }
    }

    return locations;

}
bool isAlive(int obj) {
    for(auto& entry : sprites) {
    	if(entry.second.objectPointer==obj) return true;
    } 
    return false;
}

void Silver::Threading::applyFunction(const string& name, int number, function<void(int)> func, char mode, ...) {
    vector<int> targetObjects;
    int key = 0;
    Vec3 pos = {0, 0, 0};
    va_list args;
    va_start(args, mode);

    if (mode == 'k') {
        key = va_arg(args, int);
    } else if (mode == 'm' || mode == 'c' || mode == 'C') {
        key = silver.mouse.mouseKey;
        if (mode == 'c') pos = va_arg(args, Vec3);
        if (mode == 'C') pos = silver.camera.getScreenPosition(silver.getLocation(name, number)[0]);
    } else if (mode == 'h' || mode == 'H') {
        if (mode == 'h') pos = va_arg(args, Vec3);
        if (mode == 'H') pos = silver.camera.getScreenPosition(silver.getLocation(name, number)[0]);
    }
    va_end(args);

    if (name == pointer) {
        for (const auto& entry : sprites) {
            if (entry.second.objectPointer == number) {
                targetObjects.push_back(entry.second.objectPointer);
            }
        }
    } else {
        for (const auto& entry : sprites) {
            if (entry.second.object.name == name && (number ==  all || entry.second.object.number == number)) {
                targetObjects.push_back(entry.second.objectPointer);
            }
        }
    }

    if (targetObjects.empty()) {
        return; 
    }

    for (int obj : targetObjects) {
        lock_guard<mutex> lock(thread_mutex); 

        activeThreads.emplace_back([obj, func, mode, key, pos]() {
            while (isAlive(obj)) {
                if (mode == 'k' || mode == 'm' || mode == 'c' || mode == 'C') {
                    while (true) {
                        silver.keyboard.getKey();
                        if (silver.keyboard.isKey(key)) {
                            if (mode == 'c' || mode == 'C') {
                                Vec3 cursorPosition = {silver.mouse.cursorPositionX, silver.mouse.cursorPositionY, 0};
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
                        Vec3 cursorPosition = {silver.mouse.cursorPositionX, silver.mouse.cursorPositionY, 0};
                        if (cursorPosition.x == pos.x && cursorPosition.y == pos.y) {  
                            break;
                        }

                    }
                }
                func(obj); 

            }
        });
    }
}

vector<int> Silver::getLocationY(const string& name, int number) {
    vector<int> locations; 

    for (auto& entry : sprites) {
        if(name==pointer) {
            if(entry.second.objectPointer==number) {
                locations.push_back(entry.second.object.position.y); 
            }
            continue;
        }
        if (entry.second.object.name == name && (number == all || entry.second.object.number == number)) {
            locations.push_back(entry.second.object.position.y); 
        }
    }

    return locations;
}

vector<int> Silver::getLocationZ(const string& name, int number) {
    vector<int> locations;

    for (auto& entry : sprites) {
        if(name==pointer) {
            if(entry.second.objectPointer==number) {
                locations.push_back(entry.second.object.position.z); 
            }
            continue;
        }
        if (entry.second.object.name == name && (number == all || entry.second.object.number == number)) {
            locations.push_back(entry.second.object.position.z); 
        }
    }

    return locations;
}

void Silver::setObjectPositionToSprite(const string& name, const variant<int, vector<int>>& number,
                                       const string& target, int targetNumber) {
    Vec3 targetLocation = getLocation(target, targetNumber)[0];

        if (holds_alternative<int>(number)) {
            moveObjectPosition(name, get<int>(number), targetLocation);
        } else {
            const vector<int>& numList = get<vector<int>>(number);
            for (int num : numList) {
                moveObjectPosition(name, num, targetLocation);
            }
        }

}

void Silver::glideObjectPositionToSprite(const string& name, const variant<int, vector<int>>& number,
                                         const string& target, int targetNumber, float speed) {
    Vec3 targetLocation = getLocation(target, targetNumber)[0];

        if (holds_alternative<int>(number)) {
            glideObjectXY(name, get<int>(number), targetLocation, speed);
        } else {
            const vector<int>& numList = get<vector<int>>(number);
            for (int num : numList) {
                glideObjectXY(name, num, targetLocation, speed);  
            }
        }

}

void Silver::bringToTop(const string& name, int number) {
    if (number == origin) {
        return;
    }

    auto bringSpriteToTop = [&](auto& entry) {
        mesh X = entry.second;
        X.object.position.z = 0;  
        sprites.erase(entry.first);
        sprites.insert({{X.object.position.x, X.object.position.y, 0}, X});
    };

    if (number == all) {
        for (auto& entry : sprites) {
            if (entry.second.object.name == name) {
                bringSpriteToTop(entry);
            }
        }
    } else {
        for (auto& entry : sprites) {
            if (entry.second.object.name == name && entry.second.object.number == number) {
                bringSpriteToTop(entry);
            }
        }
    }
}

void Silver::applyTag(const string& name, const variant<int, vector<int>>& number, const string& tag) {
    auto addTagToSprite = [&](auto& entry, int num) {
        if (entry.second.object.name == name && (num == all || entry.second.object.number == num)) {
            entry.second.object.tags.push_back(tag);
        }
    };

    if (holds_alternative<int>(number)) {
        int num = get<int>(number);

        if (num == origin) {
            for (auto& entry : globalObjects) {
                if (entry.second.name == name) {
                    entry.second.tags.push_back(tag);
                }
            }
        } else {
            for (auto& entry : sprites) {
                addTagToSprite(entry, num);
            }
        }
    } else {
        const vector<int>& numbers = get<vector<int>>(number);
        for (int num : numbers) {
            for (auto& entry : sprites) {
                addTagToSprite(entry, num);
            }
        }
    }
}

vector<pair<string, int>> Silver::seekTag(const string& tag) {
    vector<pair<string, int>> result;

    auto hasTag = [&](const mesh& obj) {
        return find(obj.object.tags.begin(), obj.object.tags.end(), tag) != obj.object.tags.end();
    };

    for (const auto& entry : sprites) {
        if (hasTag(entry.second)) {
            result.emplace_back(entry.second.object.name, entry.second.object.number);
        }
    }

    return result;
}

vector<string> Silver::getTag(const string& name, int number) {
    if(number==origin) {
	    for (const auto& entry : globalObjects) {
		return entry.second.tags; 
	    }
    }

    for (const auto& entry : sprites) {
        if (entry.second.object.name == name && entry.second.object.number == number) {
            return entry.second.object.tags; 
        }
    }

    return {}; 
}

zone Silver::buildZone(variant<tuple<int,int,int,int>, vector<tuple<int,int,int,int>>> shape) {
    return zone(shape);
}

bool Silver::isInZone(const zone& z, Vec3 pos) {
    int pointX=pos.x, pointY=pos.y;
    if (holds_alternative<tuple<int, int, int, int>>(z.shape)) {
        const auto& rect = get<tuple<int, int, int, int>>(z.shape);
        int x = get<0>(rect);
        int y = get<1>(rect);
        int width = get<2>(rect);
        int height = get<3>(rect);

        return (pointX >= x && pointX <= x + width) &&
               (pointY >= y && pointY <= y + height);
    } else {
        const auto& rects = get<vector<tuple<int, int, int, int>>>(z.shape);
        for (const auto& rect : rects) {
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

void Silver::Timer::resetTimer(string name) {
    running[name] = false;
    duration[name] = 0;
}

long long Silver::Timer::getDuration(string name) const {

    if (running.find(name) != running.end() && running.at(name) == true) {
        auto currentTime = chrono::high_resolution_clock::now();
        return duration.at(name) + chrono::duration_cast<chrono::milliseconds>(currentTime - startTime.at(name)).count();
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
        duration[name] += chrono::duration_cast<chrono::milliseconds>(endTime - startTime.at(name)).count();
        running[name] = false;
    }
}

float Silver::Timer::getDeltaTime(string name) const {
    return deltaTime.at(name); 
}

void Silver::Timer::update(string name) {
    auto currentTime = chrono::high_resolution_clock::now();

    deltaTime[name] = chrono::duration<float>(currentTime - lastTime.at(name)).count(); 

    lastTime[name] = currentTime;
}

void Silver::walk(string name, variant<vector<int>,int> number, int steps, int direction) {

    const double PI = 3.14159265358979323846;
    double rad = direction * (PI / 180.0);

    int deltaX = steps * sin(rad);
    int deltaY = steps * cos(rad);

    if(holds_alternative<int>(number)) {
        int singleNumber = get<int>(number);
        silver.moveObjectXY(name, singleNumber, Vec2(deltaX, deltaY));
    } else {
        vector<int> numbers = get<vector<int>>(number);
        silver.moveObjectXY(name, numbers, Vec2(deltaX, deltaY));
    }
}
