
<img src="https://github.com/imagment/Silver-Cplusplus/raw/dev/Icons/600x600.png" width="400" height="400" align="left">

**Silver C++** is a simple and easy-to-use library designed for text-based game development. You can create an interactive story game, an escape room game, a simple console adventure, and anything!  

<i>When I was a solo game developer, I used to get frustrated with having to adjust every little design detail, like resizing a logo or fixing a missing pixel on my pixel art. That’s when I started exploring text-based games.

I realized that text-based games are often underrated, but they can be incredibly rewarding with a good story and solid game mechanics. That’s why I created this library—to simplify the development process of text-based games.

Creating text-based games is incredibly rewarding, and this library not only enhances their value and productivity but also aims to make text-based games more enjoyed and accessible to many videogame enjoyers. Our goal is to let people enjoy text-based games just as much as they enjoy traditional games.</i>
<hr>

[![Discord](https://img.shields.io/badge/Discord-LoTGD-blue?logo=discord)](https://discord.gg/72TbP8G69k)
[![Release](https://img.shields.io/github/v/release/imagment/Silver-Cplusplus)](https://github.com/imagment/Silver-Cplusplus/releases)

## Installation

To install **Silver C++**, follow these steps:

1. Clone the repository:
   ```bash
   git clone https://github.com/imagment/Silver-Cplusplus.git

2. Build with CMake
After cloning the repository, navigate to the project folder and use CMake to build the project:
```
cd Silver-Cplusplus
mkdir build
cd build
cmake ..
make
```
**Important: Compatibility Information** <br>
Silver C++ is only compatible with **Linux-based** operating systems. <br>
Windows is *not* supported currently. We are creating another version of Silver C++ that supports Windows.

## Features  
- ❌ **No external dependencies** - All required libraries are included into Silver C++
- 🚀 **Simple & Lightweight** – Minimal setup required, so you can focus on game logic.
- 🎨 **Rich Text Support** – Easily display formatted text, colors, and simple animations for immersive gameplay. 
- 🎮 **Interactive Input Handling** – Seamlessly process user input for interactive storytelling and decision-making. 
- 🛠 **Easy-to-Use API** – Intuitive functions for game logic, and essential components like transform, camera and sprite rendering. 
- ⚡ **Multithreading Support** – Boost performance by running tasks concurrently. 
- 🔢 **Math Module** – Built-in utilities for Vectors and Rects
- 📝 **Code Style** – Written in C++17 using CamelCase and PascalCase notation for clarity and consistency.

## Examples
```
#include "Silver.hpp"

int main() {
  Actor c1;
  c1.AddComponent<Camera>();


  Actor actor("alert", "Hello World!");
  actor.GetComponent<Transform>()->position = Vector3Zero;
  actor.GetComponent<Transform>()->scale = Vector3(1,1,1);
  
  actor.AddObject();
  c1.GetComponent<Camera>()->RenderFrame();

  Hold();
  return 0;
}
```




