# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ishuena/heinz/heinz

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ishuena/heinz/heinz/build

# Include any dependencies generated for this target.
include CMakeFiles/heinz.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/heinz.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/heinz.dir/flags.make

CMakeFiles/heinz.dir/src/mwcs.cpp.o: CMakeFiles/heinz.dir/flags.make
CMakeFiles/heinz.dir/src/mwcs.cpp.o: ../src/mwcs.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ishuena/heinz/heinz/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/heinz.dir/src/mwcs.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/heinz.dir/src/mwcs.cpp.o -c /home/ishuena/heinz/heinz/src/mwcs.cpp

CMakeFiles/heinz.dir/src/mwcs.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/heinz.dir/src/mwcs.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ishuena/heinz/heinz/src/mwcs.cpp > CMakeFiles/heinz.dir/src/mwcs.cpp.i

CMakeFiles/heinz.dir/src/mwcs.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/heinz.dir/src/mwcs.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ishuena/heinz/heinz/src/mwcs.cpp -o CMakeFiles/heinz.dir/src/mwcs.cpp.s

CMakeFiles/heinz.dir/src/mwcs.cpp.o.requires:

.PHONY : CMakeFiles/heinz.dir/src/mwcs.cpp.o.requires

CMakeFiles/heinz.dir/src/mwcs.cpp.o.provides: CMakeFiles/heinz.dir/src/mwcs.cpp.o.requires
	$(MAKE) -f CMakeFiles/heinz.dir/build.make CMakeFiles/heinz.dir/src/mwcs.cpp.o.provides.build
.PHONY : CMakeFiles/heinz.dir/src/mwcs.cpp.o.provides

CMakeFiles/heinz.dir/src/mwcs.cpp.o.provides.build: CMakeFiles/heinz.dir/src/mwcs.cpp.o


CMakeFiles/heinz.dir/src/utils.cpp.o: CMakeFiles/heinz.dir/flags.make
CMakeFiles/heinz.dir/src/utils.cpp.o: ../src/utils.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ishuena/heinz/heinz/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/heinz.dir/src/utils.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/heinz.dir/src/utils.cpp.o -c /home/ishuena/heinz/heinz/src/utils.cpp

CMakeFiles/heinz.dir/src/utils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/heinz.dir/src/utils.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ishuena/heinz/heinz/src/utils.cpp > CMakeFiles/heinz.dir/src/utils.cpp.i

CMakeFiles/heinz.dir/src/utils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/heinz.dir/src/utils.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ishuena/heinz/heinz/src/utils.cpp -o CMakeFiles/heinz.dir/src/utils.cpp.s

CMakeFiles/heinz.dir/src/utils.cpp.o.requires:

.PHONY : CMakeFiles/heinz.dir/src/utils.cpp.o.requires

CMakeFiles/heinz.dir/src/utils.cpp.o.provides: CMakeFiles/heinz.dir/src/utils.cpp.o.requires
	$(MAKE) -f CMakeFiles/heinz.dir/build.make CMakeFiles/heinz.dir/src/utils.cpp.o.provides.build
.PHONY : CMakeFiles/heinz.dir/src/utils.cpp.o.provides

CMakeFiles/heinz.dir/src/utils.cpp.o.provides.build: CMakeFiles/heinz.dir/src/utils.cpp.o


# Object files for target heinz
heinz_OBJECTS = \
"CMakeFiles/heinz.dir/src/mwcs.cpp.o" \
"CMakeFiles/heinz.dir/src/utils.cpp.o"

# External object files for target heinz
heinz_EXTERNAL_OBJECTS =

heinz: CMakeFiles/heinz.dir/src/mwcs.cpp.o
heinz: CMakeFiles/heinz.dir/src/utils.cpp.o
heinz: CMakeFiles/heinz.dir/build.make
heinz: CMakeFiles/heinz.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ishuena/heinz/heinz/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable heinz"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/heinz.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/heinz.dir/build: heinz

.PHONY : CMakeFiles/heinz.dir/build

CMakeFiles/heinz.dir/requires: CMakeFiles/heinz.dir/src/mwcs.cpp.o.requires
CMakeFiles/heinz.dir/requires: CMakeFiles/heinz.dir/src/utils.cpp.o.requires

.PHONY : CMakeFiles/heinz.dir/requires

CMakeFiles/heinz.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/heinz.dir/cmake_clean.cmake
.PHONY : CMakeFiles/heinz.dir/clean

CMakeFiles/heinz.dir/depend:
	cd /home/ishuena/heinz/heinz/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ishuena/heinz/heinz /home/ishuena/heinz/heinz /home/ishuena/heinz/heinz/build /home/ishuena/heinz/heinz/build /home/ishuena/heinz/heinz/build/CMakeFiles/heinz.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/heinz.dir/depend

