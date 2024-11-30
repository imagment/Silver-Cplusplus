cmake_minimum_required(VERSION 3.10)

project(Silver VERSION 0.3.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(Silver STATIC ${CMAKE_SOURCE_DIR}/src/silver.cpp)
add_library(Silver.Keyboard STATIC ${CMAKE_SOURCE_DIR}/src/silver.Keyboard.cpp)
add_library(Silver.VMouse STATIC ${CMAKE_SOURCE_DIR}/src/silver.VMouse.cpp)
add_library(Silver.Timer STATIC ${CMAKE_SOURCE_DIR}/src/silver.Timer.cpp)
add_executable(MyGame ${CMAKE_SOURCE_DIR}/src/main.cpp)

target_link_libraries(MyGame PRIVATE Silver Silver.Keyboard Silver.Timer)
