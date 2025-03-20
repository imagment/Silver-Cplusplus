cmake_minimum_required(VERSION 3.10)

# Project configuration
project(Silver VERSION 0.3.6)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)


# Paths for headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Precompiled header setup
set(PCH_HEADER ${CMAKE_SOURCE_DIR}/include/Silver.hpp)
set(PCH_OUTPUT_DIR ${CMAKE_BINARY_DIR}/pch)
set(PCH_COMPILED ${PCH_OUTPUT_DIR}/Silver_pch.hpp.gch)

add_custom_command(
    OUTPUT ${PCH_COMPILED}
    COMMAND ${CMAKE_CXX_COMPILER} -std=c++17 -x c++-header -o ${PCH_COMPILED} ${PCH_HEADER}
    DEPENDS ${PCH_HEADER}
)

# ---------- Library: Silver ----------
# Include all .cpp files from the src directory
file(GLOB SILVER_SRC ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_library(Silver STATIC ${SILVER_SRC})

# Precompiled header for Silver library
target_compile_options(Silver PRIVATE -include ${PCH_HEADER})

# ---------- Include miniaudio ----------
# Include miniaudio header (no need to link anything separately)
include_directories(${CMAKE_SOURCE_DIR}/include)

# ---------- Executable: MyGame ----------
# Include all .cpp files from the src directory for the executable
file(GLOB MYGAME_SRC ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(MyGame ${MYGAME_SRC})

# Precompiled header for MyGame executable
target_compile_options(MyGame PRIVATE -include ${PCH_HEADER})

# Link libraries to MyGame
target_link_libraries(MyGame PRIVATE
    Silver       # Link to your static library
    pthread      # POSIX threading support
)

# ---------- Build Messages ----------
message(STATUS "Miniaudio included from: ${CMAKE_SOURCE_DIR}/include/miniaudio.h")

