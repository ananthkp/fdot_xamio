# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /home/ucal/Mount/fdot/esesc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ucal/Mount/fdot/build

# Include any dependencies generated for this target.
include tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/depend.make

# Include the progress variables for this target.
include tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/progress.make

# Include the compile flags for this target's objects.
include tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/flags.make

tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o: tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/flags.make
tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o: /home/ucal/Mount/fdot/esesc/tests/gtest-1.7.0/src/gtest_main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o"
	cd /home/ucal/Mount/fdot/build/tests/gtest-1.7.0 && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gtest_main.dir/src/gtest_main.cc.o -c /home/ucal/Mount/fdot/esesc/tests/gtest-1.7.0/src/gtest_main.cc

tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gtest_main.dir/src/gtest_main.cc.i"
	cd /home/ucal/Mount/fdot/build/tests/gtest-1.7.0 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ucal/Mount/fdot/esesc/tests/gtest-1.7.0/src/gtest_main.cc > CMakeFiles/gtest_main.dir/src/gtest_main.cc.i

tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gtest_main.dir/src/gtest_main.cc.s"
	cd /home/ucal/Mount/fdot/build/tests/gtest-1.7.0 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ucal/Mount/fdot/esesc/tests/gtest-1.7.0/src/gtest_main.cc -o CMakeFiles/gtest_main.dir/src/gtest_main.cc.s

tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o.requires:

.PHONY : tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o.requires

tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o.provides: tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o.requires
	$(MAKE) -f tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/build.make tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o.provides.build
.PHONY : tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o.provides

tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o.provides.build: tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o


# Object files for target gtest_main
gtest_main_OBJECTS = \
"CMakeFiles/gtest_main.dir/src/gtest_main.cc.o"

# External object files for target gtest_main
gtest_main_EXTERNAL_OBJECTS =

tests/gtest-1.7.0/libgtest_main.a: tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o
tests/gtest-1.7.0/libgtest_main.a: tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/build.make
tests/gtest-1.7.0/libgtest_main.a: tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libgtest_main.a"
	cd /home/ucal/Mount/fdot/build/tests/gtest-1.7.0 && $(CMAKE_COMMAND) -P CMakeFiles/gtest_main.dir/cmake_clean_target.cmake
	cd /home/ucal/Mount/fdot/build/tests/gtest-1.7.0 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gtest_main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/build: tests/gtest-1.7.0/libgtest_main.a

.PHONY : tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/build

tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/requires: tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o.requires

.PHONY : tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/requires

tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/clean:
	cd /home/ucal/Mount/fdot/build/tests/gtest-1.7.0 && $(CMAKE_COMMAND) -P CMakeFiles/gtest_main.dir/cmake_clean.cmake
.PHONY : tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/clean

tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/depend:
	cd /home/ucal/Mount/fdot/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ucal/Mount/fdot/esesc /home/ucal/Mount/fdot/esesc/tests/gtest-1.7.0 /home/ucal/Mount/fdot/build /home/ucal/Mount/fdot/build/tests/gtest-1.7.0 /home/ucal/Mount/fdot/build/tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/gtest-1.7.0/CMakeFiles/gtest_main.dir/depend

