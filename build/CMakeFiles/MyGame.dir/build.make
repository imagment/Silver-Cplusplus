# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/imagment/Desktop/555

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/imagment/Desktop/555/build

# Include any dependencies generated for this target.
include CMakeFiles/MyGame.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/MyGame.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/MyGame.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MyGame.dir/flags.make

CMakeFiles/MyGame.dir/src/main.cpp.o: CMakeFiles/MyGame.dir/flags.make
CMakeFiles/MyGame.dir/src/main.cpp.o: /home/imagment/Desktop/555/src/main.cpp
CMakeFiles/MyGame.dir/src/main.cpp.o: CMakeFiles/MyGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/MyGame.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/MyGame.dir/src/main.cpp.o -MF CMakeFiles/MyGame.dir/src/main.cpp.o.d -o CMakeFiles/MyGame.dir/src/main.cpp.o -c /home/imagment/Desktop/555/src/main.cpp

CMakeFiles/MyGame.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/MyGame.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imagment/Desktop/555/src/main.cpp > CMakeFiles/MyGame.dir/src/main.cpp.i

CMakeFiles/MyGame.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/MyGame.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imagment/Desktop/555/src/main.cpp -o CMakeFiles/MyGame.dir/src/main.cpp.s

CMakeFiles/MyGame.dir/src/silver.Animation.cpp.o: CMakeFiles/MyGame.dir/flags.make
CMakeFiles/MyGame.dir/src/silver.Animation.cpp.o: /home/imagment/Desktop/555/src/silver.Animation.cpp
CMakeFiles/MyGame.dir/src/silver.Animation.cpp.o: CMakeFiles/MyGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/MyGame.dir/src/silver.Animation.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/MyGame.dir/src/silver.Animation.cpp.o -MF CMakeFiles/MyGame.dir/src/silver.Animation.cpp.o.d -o CMakeFiles/MyGame.dir/src/silver.Animation.cpp.o -c /home/imagment/Desktop/555/src/silver.Animation.cpp

CMakeFiles/MyGame.dir/src/silver.Animation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/MyGame.dir/src/silver.Animation.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imagment/Desktop/555/src/silver.Animation.cpp > CMakeFiles/MyGame.dir/src/silver.Animation.cpp.i

CMakeFiles/MyGame.dir/src/silver.Animation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/MyGame.dir/src/silver.Animation.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imagment/Desktop/555/src/silver.Animation.cpp -o CMakeFiles/MyGame.dir/src/silver.Animation.cpp.s

CMakeFiles/MyGame.dir/src/silver.Camera.cpp.o: CMakeFiles/MyGame.dir/flags.make
CMakeFiles/MyGame.dir/src/silver.Camera.cpp.o: /home/imagment/Desktop/555/src/silver.Camera.cpp
CMakeFiles/MyGame.dir/src/silver.Camera.cpp.o: CMakeFiles/MyGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/MyGame.dir/src/silver.Camera.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/MyGame.dir/src/silver.Camera.cpp.o -MF CMakeFiles/MyGame.dir/src/silver.Camera.cpp.o.d -o CMakeFiles/MyGame.dir/src/silver.Camera.cpp.o -c /home/imagment/Desktop/555/src/silver.Camera.cpp

CMakeFiles/MyGame.dir/src/silver.Camera.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/MyGame.dir/src/silver.Camera.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imagment/Desktop/555/src/silver.Camera.cpp > CMakeFiles/MyGame.dir/src/silver.Camera.cpp.i

CMakeFiles/MyGame.dir/src/silver.Camera.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/MyGame.dir/src/silver.Camera.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imagment/Desktop/555/src/silver.Camera.cpp -o CMakeFiles/MyGame.dir/src/silver.Camera.cpp.s

CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.o: CMakeFiles/MyGame.dir/flags.make
CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.o: /home/imagment/Desktop/555/src/silver.Fluid.cpp
CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.o: CMakeFiles/MyGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.o -MF CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.o.d -o CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.o -c /home/imagment/Desktop/555/src/silver.Fluid.cpp

CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imagment/Desktop/555/src/silver.Fluid.cpp > CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.i

CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imagment/Desktop/555/src/silver.Fluid.cpp -o CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.s

CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.o: CMakeFiles/MyGame.dir/flags.make
CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.o: /home/imagment/Desktop/555/src/silver.Keyboard.cpp
CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.o: CMakeFiles/MyGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.o -MF CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.o.d -o CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.o -c /home/imagment/Desktop/555/src/silver.Keyboard.cpp

CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imagment/Desktop/555/src/silver.Keyboard.cpp > CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.i

CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imagment/Desktop/555/src/silver.Keyboard.cpp -o CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.s

CMakeFiles/MyGame.dir/src/silver.Music.cpp.o: CMakeFiles/MyGame.dir/flags.make
CMakeFiles/MyGame.dir/src/silver.Music.cpp.o: /home/imagment/Desktop/555/src/silver.Music.cpp
CMakeFiles/MyGame.dir/src/silver.Music.cpp.o: CMakeFiles/MyGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/MyGame.dir/src/silver.Music.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/MyGame.dir/src/silver.Music.cpp.o -MF CMakeFiles/MyGame.dir/src/silver.Music.cpp.o.d -o CMakeFiles/MyGame.dir/src/silver.Music.cpp.o -c /home/imagment/Desktop/555/src/silver.Music.cpp

CMakeFiles/MyGame.dir/src/silver.Music.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/MyGame.dir/src/silver.Music.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imagment/Desktop/555/src/silver.Music.cpp > CMakeFiles/MyGame.dir/src/silver.Music.cpp.i

CMakeFiles/MyGame.dir/src/silver.Music.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/MyGame.dir/src/silver.Music.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imagment/Desktop/555/src/silver.Music.cpp -o CMakeFiles/MyGame.dir/src/silver.Music.cpp.s

CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.o: CMakeFiles/MyGame.dir/flags.make
CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.o: /home/imagment/Desktop/555/src/silver.SpriteRendering.cpp
CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.o: CMakeFiles/MyGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.o -MF CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.o.d -o CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.o -c /home/imagment/Desktop/555/src/silver.SpriteRendering.cpp

CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imagment/Desktop/555/src/silver.SpriteRendering.cpp > CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.i

CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imagment/Desktop/555/src/silver.SpriteRendering.cpp -o CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.s

CMakeFiles/MyGame.dir/src/silver.Threading.cpp.o: CMakeFiles/MyGame.dir/flags.make
CMakeFiles/MyGame.dir/src/silver.Threading.cpp.o: /home/imagment/Desktop/555/src/silver.Threading.cpp
CMakeFiles/MyGame.dir/src/silver.Threading.cpp.o: CMakeFiles/MyGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/MyGame.dir/src/silver.Threading.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/MyGame.dir/src/silver.Threading.cpp.o -MF CMakeFiles/MyGame.dir/src/silver.Threading.cpp.o.d -o CMakeFiles/MyGame.dir/src/silver.Threading.cpp.o -c /home/imagment/Desktop/555/src/silver.Threading.cpp

CMakeFiles/MyGame.dir/src/silver.Threading.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/MyGame.dir/src/silver.Threading.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imagment/Desktop/555/src/silver.Threading.cpp > CMakeFiles/MyGame.dir/src/silver.Threading.cpp.i

CMakeFiles/MyGame.dir/src/silver.Threading.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/MyGame.dir/src/silver.Threading.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imagment/Desktop/555/src/silver.Threading.cpp -o CMakeFiles/MyGame.dir/src/silver.Threading.cpp.s

CMakeFiles/MyGame.dir/src/silver.Timer.cpp.o: CMakeFiles/MyGame.dir/flags.make
CMakeFiles/MyGame.dir/src/silver.Timer.cpp.o: /home/imagment/Desktop/555/src/silver.Timer.cpp
CMakeFiles/MyGame.dir/src/silver.Timer.cpp.o: CMakeFiles/MyGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/MyGame.dir/src/silver.Timer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/MyGame.dir/src/silver.Timer.cpp.o -MF CMakeFiles/MyGame.dir/src/silver.Timer.cpp.o.d -o CMakeFiles/MyGame.dir/src/silver.Timer.cpp.o -c /home/imagment/Desktop/555/src/silver.Timer.cpp

CMakeFiles/MyGame.dir/src/silver.Timer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/MyGame.dir/src/silver.Timer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imagment/Desktop/555/src/silver.Timer.cpp > CMakeFiles/MyGame.dir/src/silver.Timer.cpp.i

CMakeFiles/MyGame.dir/src/silver.Timer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/MyGame.dir/src/silver.Timer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imagment/Desktop/555/src/silver.Timer.cpp -o CMakeFiles/MyGame.dir/src/silver.Timer.cpp.s

CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.o: CMakeFiles/MyGame.dir/flags.make
CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.o: /home/imagment/Desktop/555/src/silver.VMouse.cpp
CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.o: CMakeFiles/MyGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.o -MF CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.o.d -o CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.o -c /home/imagment/Desktop/555/src/silver.VMouse.cpp

CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imagment/Desktop/555/src/silver.VMouse.cpp > CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.i

CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imagment/Desktop/555/src/silver.VMouse.cpp -o CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.s

CMakeFiles/MyGame.dir/src/silver.cpp.o: CMakeFiles/MyGame.dir/flags.make
CMakeFiles/MyGame.dir/src/silver.cpp.o: /home/imagment/Desktop/555/src/silver.cpp
CMakeFiles/MyGame.dir/src/silver.cpp.o: CMakeFiles/MyGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/MyGame.dir/src/silver.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/MyGame.dir/src/silver.cpp.o -MF CMakeFiles/MyGame.dir/src/silver.cpp.o.d -o CMakeFiles/MyGame.dir/src/silver.cpp.o -c /home/imagment/Desktop/555/src/silver.cpp

CMakeFiles/MyGame.dir/src/silver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/MyGame.dir/src/silver.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imagment/Desktop/555/src/silver.cpp > CMakeFiles/MyGame.dir/src/silver.cpp.i

CMakeFiles/MyGame.dir/src/silver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/MyGame.dir/src/silver.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imagment/Desktop/555/src/silver.cpp -o CMakeFiles/MyGame.dir/src/silver.cpp.s

CMakeFiles/MyGame.dir/src/smath.cpp.o: CMakeFiles/MyGame.dir/flags.make
CMakeFiles/MyGame.dir/src/smath.cpp.o: /home/imagment/Desktop/555/src/smath.cpp
CMakeFiles/MyGame.dir/src/smath.cpp.o: CMakeFiles/MyGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/MyGame.dir/src/smath.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/MyGame.dir/src/smath.cpp.o -MF CMakeFiles/MyGame.dir/src/smath.cpp.o.d -o CMakeFiles/MyGame.dir/src/smath.cpp.o -c /home/imagment/Desktop/555/src/smath.cpp

CMakeFiles/MyGame.dir/src/smath.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/MyGame.dir/src/smath.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imagment/Desktop/555/src/smath.cpp > CMakeFiles/MyGame.dir/src/smath.cpp.i

CMakeFiles/MyGame.dir/src/smath.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/MyGame.dir/src/smath.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imagment/Desktop/555/src/smath.cpp -o CMakeFiles/MyGame.dir/src/smath.cpp.s

# Object files for target MyGame
MyGame_OBJECTS = \
"CMakeFiles/MyGame.dir/src/main.cpp.o" \
"CMakeFiles/MyGame.dir/src/silver.Animation.cpp.o" \
"CMakeFiles/MyGame.dir/src/silver.Camera.cpp.o" \
"CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.o" \
"CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.o" \
"CMakeFiles/MyGame.dir/src/silver.Music.cpp.o" \
"CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.o" \
"CMakeFiles/MyGame.dir/src/silver.Threading.cpp.o" \
"CMakeFiles/MyGame.dir/src/silver.Timer.cpp.o" \
"CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.o" \
"CMakeFiles/MyGame.dir/src/silver.cpp.o" \
"CMakeFiles/MyGame.dir/src/smath.cpp.o"

# External object files for target MyGame
MyGame_EXTERNAL_OBJECTS =

MyGame: CMakeFiles/MyGame.dir/src/main.cpp.o
MyGame: CMakeFiles/MyGame.dir/src/silver.Animation.cpp.o
MyGame: CMakeFiles/MyGame.dir/src/silver.Camera.cpp.o
MyGame: CMakeFiles/MyGame.dir/src/silver.Fluid.cpp.o
MyGame: CMakeFiles/MyGame.dir/src/silver.Keyboard.cpp.o
MyGame: CMakeFiles/MyGame.dir/src/silver.Music.cpp.o
MyGame: CMakeFiles/MyGame.dir/src/silver.SpriteRendering.cpp.o
MyGame: CMakeFiles/MyGame.dir/src/silver.Threading.cpp.o
MyGame: CMakeFiles/MyGame.dir/src/silver.Timer.cpp.o
MyGame: CMakeFiles/MyGame.dir/src/silver.VMouse.cpp.o
MyGame: CMakeFiles/MyGame.dir/src/silver.cpp.o
MyGame: CMakeFiles/MyGame.dir/src/smath.cpp.o
MyGame: CMakeFiles/MyGame.dir/build.make
MyGame: libSilver.a
MyGame: CMakeFiles/MyGame.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Linking CXX executable MyGame"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MyGame.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MyGame.dir/build: MyGame
.PHONY : CMakeFiles/MyGame.dir/build

CMakeFiles/MyGame.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MyGame.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MyGame.dir/clean

CMakeFiles/MyGame.dir/depend:
	cd /home/imagment/Desktop/555/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/imagment/Desktop/555 /home/imagment/Desktop/555 /home/imagment/Desktop/555/build /home/imagment/Desktop/555/build /home/imagment/Desktop/555/build/CMakeFiles/MyGame.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/MyGame.dir/depend

