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
include CMakeFiles/Silver.Keyboard.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Silver.Keyboard.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Silver.Keyboard.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Silver.Keyboard.dir/flags.make

CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.o: CMakeFiles/Silver.Keyboard.dir/flags.make
CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.o: /home/imagment/Desktop/555/src/silver.Keyboard.cpp
CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.o: CMakeFiles/Silver.Keyboard.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.o -MF CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.o.d -o CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.o -c /home/imagment/Desktop/555/src/silver.Keyboard.cpp

CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imagment/Desktop/555/src/silver.Keyboard.cpp > CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.i

CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imagment/Desktop/555/src/silver.Keyboard.cpp -o CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.s

# Object files for target Silver.Keyboard
Silver_Keyboard_OBJECTS = \
"CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.o"

# External object files for target Silver.Keyboard
Silver_Keyboard_EXTERNAL_OBJECTS =

libSilver.Keyboard.a: CMakeFiles/Silver.Keyboard.dir/src/silver.Keyboard.cpp.o
libSilver.Keyboard.a: CMakeFiles/Silver.Keyboard.dir/build.make
libSilver.Keyboard.a: CMakeFiles/Silver.Keyboard.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/imagment/Desktop/555/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libSilver.Keyboard.a"
	$(CMAKE_COMMAND) -P CMakeFiles/Silver.Keyboard.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Silver.Keyboard.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Silver.Keyboard.dir/build: libSilver.Keyboard.a
.PHONY : CMakeFiles/Silver.Keyboard.dir/build

CMakeFiles/Silver.Keyboard.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Silver.Keyboard.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Silver.Keyboard.dir/clean

CMakeFiles/Silver.Keyboard.dir/depend:
	cd /home/imagment/Desktop/555/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/imagment/Desktop/555 /home/imagment/Desktop/555 /home/imagment/Desktop/555/build /home/imagment/Desktop/555/build /home/imagment/Desktop/555/build/CMakeFiles/Silver.Keyboard.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/Silver.Keyboard.dir/depend

