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

# Utility rule file for regression.

# Include the progress variables for this target.
include CMakeFiles/regression.dir/progress.make

CMakeFiles/regression: CMakeFiles/regression-complete


CMakeFiles/regression-complete: gold/stampdir/regression-install
CMakeFiles/regression-complete: gold/stampdir/regression-mkdir
CMakeFiles/regression-complete: gold/stampdir/regression-download
CMakeFiles/regression-complete: gold/stampdir/regression-update
CMakeFiles/regression-complete: gold/stampdir/regression-patch
CMakeFiles/regression-complete: gold/stampdir/regression-configure
CMakeFiles/regression-complete: gold/stampdir/regression-build
CMakeFiles/regression-complete: gold/stampdir/regression-install
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Completed 'regression'"
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/build/CMakeFiles
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/CMakeFiles/regression-complete
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/gold/stampdir/regression-done

gold/stampdir/regression-install: gold/stampdir/regression-build
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "No install step for 'regression'"
	cd /home/ucal/Mount/fdot/build/gold && /usr/bin/cmake -E echo_append
	cd /home/ucal/Mount/fdot/build/gold && /usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/gold/stampdir/regression-install

gold/stampdir/regression-mkdir:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Creating directories for 'regression'"
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/esesc/gold
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/build/gold
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/esesc/gold
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/build/gold/tmp
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/build/gold/stampdir
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/esesc/gold/src
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/gold/stampdir/regression-mkdir

gold/stampdir/regression-download: gold/stampdir/regression-mkdir
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "No download step for 'regression'"
	cd /home/ucal/Mount/fdot/esesc/gold/src && /usr/bin/cmake -E echo_append
	cd /home/ucal/Mount/fdot/esesc/gold/src && /usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/gold/stampdir/regression-download

gold/stampdir/regression-update: gold/stampdir/regression-download
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "No update step for 'regression'"
	/usr/bin/cmake -E echo_append
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/gold/stampdir/regression-update

gold/stampdir/regression-patch: gold/stampdir/regression-download
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "No patch step for 'regression'"
	/usr/bin/cmake -E echo_append
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/gold/stampdir/regression-patch

gold/stampdir/regression-configure: gold/tmp/regression-cfgcmd.txt
gold/stampdir/regression-configure: gold/stampdir/regression-update
gold/stampdir/regression-configure: gold/stampdir/regression-patch
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Performing configure step for 'regression'"
	cd /home/ucal/Mount/fdot/build/gold && /home/ucal/Mount/fdot/esesc/gold/configure /home/ucal/Mount/fdot/esesc/gold /home/ucal/Mount/fdot/build/gold
	cd /home/ucal/Mount/fdot/build/gold && /usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/gold/stampdir/regression-configure

gold/stampdir/regression-build: gold/stampdir/regression-configure
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Performing build step for 'regression'"
	cd /home/ucal/Mount/fdot/build/gold && sh gen-regression.sh
	cd /home/ucal/Mount/fdot/build/gold && /usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/gold/stampdir/regression-build

regression: CMakeFiles/regression
regression: CMakeFiles/regression-complete
regression: gold/stampdir/regression-install
regression: gold/stampdir/regression-mkdir
regression: gold/stampdir/regression-download
regression: gold/stampdir/regression-update
regression: gold/stampdir/regression-patch
regression: gold/stampdir/regression-configure
regression: gold/stampdir/regression-build
regression: CMakeFiles/regression.dir/build.make

.PHONY : regression

# Rule to build all files generated by this target.
CMakeFiles/regression.dir/build: regression

.PHONY : CMakeFiles/regression.dir/build

CMakeFiles/regression.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/regression.dir/cmake_clean.cmake
.PHONY : CMakeFiles/regression.dir/clean

CMakeFiles/regression.dir/depend:
	cd /home/ucal/Mount/fdot/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ucal/Mount/fdot/esesc /home/ucal/Mount/fdot/esesc /home/ucal/Mount/fdot/build /home/ucal/Mount/fdot/build /home/ucal/Mount/fdot/build/CMakeFiles/regression.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/regression.dir/depend
