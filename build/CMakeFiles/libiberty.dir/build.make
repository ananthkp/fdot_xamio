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

# Utility rule file for libiberty.

# Include the progress variables for this target.
include CMakeFiles/libiberty.dir/progress.make

CMakeFiles/libiberty: CMakeFiles/libiberty-complete


CMakeFiles/libiberty-complete: libiberty/stampdir/libiberty-install
CMakeFiles/libiberty-complete: libiberty/stampdir/libiberty-mkdir
CMakeFiles/libiberty-complete: libiberty/stampdir/libiberty-download
CMakeFiles/libiberty-complete: libiberty/stampdir/libiberty-update
CMakeFiles/libiberty-complete: libiberty/stampdir/libiberty-patch
CMakeFiles/libiberty-complete: libiberty/stampdir/libiberty-configure
CMakeFiles/libiberty-complete: libiberty/stampdir/libiberty-build
CMakeFiles/libiberty-complete: libiberty/stampdir/libiberty-install
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Completed 'libiberty'"
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/build/CMakeFiles
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/CMakeFiles/libiberty-complete
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/libiberty/stampdir/libiberty-done

libiberty/stampdir/libiberty-install: libiberty/stampdir/libiberty-build
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Performing install step for 'libiberty'"
	cd /home/ucal/Mount/fdot/build/libiberty && $(MAKE) install
	cd /home/ucal/Mount/fdot/build/libiberty && /usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/libiberty/stampdir/libiberty-install

libiberty/stampdir/libiberty-mkdir:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Creating directories for 'libiberty'"
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/esesc/misc/libiberty
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/build/libiberty
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/esesc/misc/libiberty
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/build/libiberty/tmp
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/build/libiberty/stampdir
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/esesc/misc/libiberty/src
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/libiberty/stampdir/libiberty-mkdir

libiberty/stampdir/libiberty-download: libiberty/stampdir/libiberty-mkdir
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "No download step for 'libiberty'"
	cd /home/ucal/Mount/fdot/esesc/misc/libiberty/src && /usr/bin/cmake -E echo_append
	cd /home/ucal/Mount/fdot/esesc/misc/libiberty/src && /usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/libiberty/stampdir/libiberty-download

libiberty/stampdir/libiberty-update: libiberty/stampdir/libiberty-download
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "No update step for 'libiberty'"
	/usr/bin/cmake -E echo_append
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/libiberty/stampdir/libiberty-update

libiberty/stampdir/libiberty-patch: libiberty/stampdir/libiberty-download
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "No patch step for 'libiberty'"
	/usr/bin/cmake -E echo_append
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/libiberty/stampdir/libiberty-patch

libiberty/stampdir/libiberty-configure: bfd/stampdir/bfd-done
libiberty/stampdir/libiberty-configure: libiberty/tmp/libiberty-cfgcmd.txt
libiberty/stampdir/libiberty-configure: libiberty/stampdir/libiberty-update
libiberty/stampdir/libiberty-configure: libiberty/stampdir/libiberty-patch
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Performing configure step for 'libiberty'"
	cd /home/ucal/Mount/fdot/build/libiberty && /home/ucal/Mount/fdot/esesc/misc/libiberty/configure --prefix=/home/ucal/Mount/fdot/build/libiberty "CFLAGS= -I/home/ucal/Mount/fdot/esesc/misc/include"
	cd /home/ucal/Mount/fdot/build/libiberty && /usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/libiberty/stampdir/libiberty-configure

libiberty/stampdir/libiberty-build: libiberty/stampdir/libiberty-configure
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Performing build step for 'libiberty'"
	cd /home/ucal/Mount/fdot/build/libiberty && $(MAKE)
	cd /home/ucal/Mount/fdot/build/libiberty && /usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/libiberty/stampdir/libiberty-build

libiberty: CMakeFiles/libiberty
libiberty: CMakeFiles/libiberty-complete
libiberty: libiberty/stampdir/libiberty-install
libiberty: libiberty/stampdir/libiberty-mkdir
libiberty: libiberty/stampdir/libiberty-download
libiberty: libiberty/stampdir/libiberty-update
libiberty: libiberty/stampdir/libiberty-patch
libiberty: libiberty/stampdir/libiberty-configure
libiberty: libiberty/stampdir/libiberty-build
libiberty: CMakeFiles/libiberty.dir/build.make

.PHONY : libiberty

# Rule to build all files generated by this target.
CMakeFiles/libiberty.dir/build: libiberty

.PHONY : CMakeFiles/libiberty.dir/build

CMakeFiles/libiberty.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/libiberty.dir/cmake_clean.cmake
.PHONY : CMakeFiles/libiberty.dir/clean

CMakeFiles/libiberty.dir/depend:
	cd /home/ucal/Mount/fdot/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ucal/Mount/fdot/esesc /home/ucal/Mount/fdot/esesc /home/ucal/Mount/fdot/build /home/ucal/Mount/fdot/build /home/ucal/Mount/fdot/build/CMakeFiles/libiberty.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/libiberty.dir/depend

