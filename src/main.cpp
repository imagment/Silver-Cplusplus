#include "silver.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  Actor c1, c2; 
c1.AddComponent<Camera>();
  Vector3 nop;

  Actor x("test", "123\n456");
 
  x.GetComponent<Transform>()->position = Vector3(0, 0, 0);

  int t = 0;

  x.GetComponent<Transform>()->rotation = t;

  x.AddObject();

 
   c1.GetComponent<Camera>()->RenderFrame();

  Hold();
  return 0;
}
