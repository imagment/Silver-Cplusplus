#include "Silver.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  // Use std::shared_ptr for c1 to ensure shared_from_this works
  SPActor c1 = std::make_shared<Actor>();
  c1->AddComponent<Camera>();
  
  SPActor test = std::make_shared<Actor>("test", "1");
  Rectangle(test, Rect(0,0,5,5), 0);
  c1->GetComponent<Camera>()->RenderFrame();
  Hold();
  return 0;
}
