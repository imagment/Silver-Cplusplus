#include <stdio.h>
#include <iostream>
#include "game.hpp"
#include <vector>

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

void printCenter(string s) {
    int x=silver.getConsoleCenter().x;
    int y=silver.getConsoleCenter().y;
    vector<string> lines = splitLines(s);  
    printf("[%d %d]",x,y);
    int startY = y - (lines.size() / 2);   

    for (int i = 0; i < lines.size(); ++i) {
        int len = lines[i].length();
        int startX = x - (len/2);       
        printf("\033[%d;%dH", startY+i, startX);   
        cout << lines[i] << flush;                 
    }
}

int progress=0;
void load(int x) {
    for(int i=0; i<x; i++){
	    int y=silver.getConsoleCenter().y+2;
	    int x=silver.getConsoleCenter().x-19+progress;
	    
	    printf("\033[%d;%dH", y, x);   
	    cout << "#" << flush;
	    progress++;
	    
	    y=silver.getConsoleCenter().y+5;
	    x=silver.getConsoleCenter().x+5;
	    
	    printf("\033[%d;%dH", y, x);   
	    int percent=(int)(((double)100.0*progress/40));
	    if(progress==39) printf("100%]");
	    else if(percent>=0 && percent<100) printf("%.2d%%",percent);
	    else if(percent==100) printf("100%]");
	    silver.wait(100);
    }
}
int main() {
/*
    silver.clear();
    printCenter(
"  _________.__.__                      \n"                     
" /   _____/|__|  |___  __ ___________  \n"
" \\_____  \\ |  |  |\\  \\/ // __ \\_  __ \\ \n"
" /        \\|  |  |_\\   /\\  ___/|  | \\/ \n"
"/_______  /|__|____/\\_/  \\___  >__|    \n"
"        \\/                   \\/     \n"
"    In silver, the worlds deliver   \n"
"  ________________________________________\n"
" /                                        \\\n"
" ==========================================\n"
"\n"
"Loading.... [00%]\n"
"Version Num: 1002\n"
    
    );
    
    load(5);
    silver.wait(1000);
    load(6);
    silver.wait(1000);
    load(9);
    silver.wait(1000);
    load(7);
    silver.wait(1500);
    load(9);
    silver.wait(1000);
    load(4);
    
    
    silver.wait(1000000);
    */
    silver.camera.setCam3(Vec3(5,5,5),Vec3(6,6,12));
    silver.createObject("test", "T");
    silver.place("test",1,Vec3(5,5,1));
    silver.place("test",1,Vec3(5,4,1));
    silver.place("test",8,Vec3(5,5,1));
    vector <int> v = silver.Seek("test");
    
    for (auto t : v) {
        cout << t << " ";
    }
    silver.camera.printCam();
}
