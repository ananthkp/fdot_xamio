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

# Utility rule file for qemu.

# Include the progress variables for this target.
include CMakeFiles/qemu.dir/progress.make

CMakeFiles/qemu: CMakeFiles/qemu-complete


CMakeFiles/qemu-complete: qemu/stampdir/qemu-install
CMakeFiles/qemu-complete: qemu/stampdir/qemu-mkdir
CMakeFiles/qemu-complete: qemu/stampdir/qemu-download
CMakeFiles/qemu-complete: qemu/stampdir/qemu-update
CMakeFiles/qemu-complete: qemu/stampdir/qemu-patch
CMakeFiles/qemu-complete: qemu/stampdir/qemu-configure
CMakeFiles/qemu-complete: qemu/stampdir/qemu-build
CMakeFiles/qemu-complete: qemu/stampdir/qemu-install
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Completed 'qemu'"
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/build/CMakeFiles
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/CMakeFiles/qemu-complete
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/qemu/stampdir/qemu-done

qemu/stampdir/qemu-install: qemu/stampdir/qemu-build
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Performing install step for 'qemu'"
	cd /home/ucal/Mount/fdot/build/qemu && $(MAKE) install
	cd /home/ucal/Mount/fdot/build/qemu && /usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/qemu/stampdir/qemu-install

qemu/stampdir/qemu-mkdir:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Creating directories for 'qemu'"
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/esesc/emul/qemu
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/build/qemu
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/esesc/emul/qemu
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/build/qemu/tmp
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/build/qemu/stampdir
	/usr/bin/cmake -E make_directory /home/ucal/Mount/fdot/esesc/emul/qemu/src
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/qemu/stampdir/qemu-mkdir

qemu/stampdir/qemu-download: qemu/stampdir/qemu-mkdir
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "No download step for 'qemu'"
	cd /home/ucal/Mount/fdot/esesc/emul/qemu/src && /usr/bin/cmake -E echo_append
	cd /home/ucal/Mount/fdot/esesc/emul/qemu/src && /usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/qemu/stampdir/qemu-download

qemu/stampdir/qemu-update: qemu/stampdir/qemu-download
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "No update step for 'qemu'"
	/usr/bin/cmake -E echo_append
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/qemu/stampdir/qemu-update

qemu/stampdir/qemu-patch: qemu/stampdir/qemu-download
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "No patch step for 'qemu'"
	/usr/bin/cmake -E echo_append
	/usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/qemu/stampdir/qemu-patch

qemu/stampdir/qemu-configure: qemu/tmp/qemu-cfgcmd.txt
qemu/stampdir/qemu-configure: qemu/stampdir/qemu-update
qemu/stampdir/qemu-configure: qemu/stampdir/qemu-patch
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Performing configure step for 'qemu'"
	cd /home/ucal/Mount/fdot/build/qemu && /home/ucal/Mount/fdot/esesc/emul/qemu/configure --prefix=/home/ucal/Mount/fdot/build/qemu --target-list=mips64el-linux-user --disable-virtfs --disable-numa --disable-nettle --disable-gcrypt --disable-bzip2 --disable-libssh2 --disable-curl --disable-linux-aio --disable-vhdx --disable-sdl --disable-docs --enable-esesc --python=python2 --disable-seccomp --disable-spice --disable-tools --disable-werror --disable-guest-agent --disable-gtk --disable-libusb --disable-libnfs --disable-xen --disable-snappy --disable-lzo --disable-smartcard --disable-gnutls --disable-vnc --disable-bluez --cc=/usr/bin/cc
	cd /home/ucal/Mount/fdot/build/qemu && /usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/qemu/stampdir/qemu-configure

qemu/stampdir/qemu-build: qemu/stampdir/qemu-configure
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Performing build step for 'qemu'"
	cd /home/ucal/Mount/fdot/build/qemu && $(MAKE)
	cd /home/ucal/Mount/fdot/build/qemu && /usr/bin/cmake -E touch /home/ucal/Mount/fdot/build/qemu/stampdir/qemu-build

qemu: CMakeFiles/qemu
qemu: CMakeFiles/qemu-complete
qemu: qemu/stampdir/qemu-install
qemu: qemu/stampdir/qemu-mkdir
qemu: qemu/stampdir/qemu-download
qemu: qemu/stampdir/qemu-update
qemu: qemu/stampdir/qemu-patch
qemu: qemu/stampdir/qemu-configure
qemu: qemu/stampdir/qemu-build
qemu: CMakeFiles/qemu.dir/build.make

.PHONY : qemu

# Rule to build all files generated by this target.
CMakeFiles/qemu.dir/build: qemu

.PHONY : CMakeFiles/qemu.dir/build

CMakeFiles/qemu.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/qemu.dir/cmake_clean.cmake
.PHONY : CMakeFiles/qemu.dir/clean

CMakeFiles/qemu.dir/depend:
	cd /home/ucal/Mount/fdot/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ucal/Mount/fdot/esesc /home/ucal/Mount/fdot/esesc /home/ucal/Mount/fdot/build /home/ucal/Mount/fdot/build /home/ucal/Mount/fdot/build/CMakeFiles/qemu.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/qemu.dir/depend

