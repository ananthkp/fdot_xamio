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
include main/CMakeFiles/cachebench.dir/depend.make

# Include the progress variables for this target.
include main/CMakeFiles/cachebench.dir/progress.make

# Include the compile flags for this target's objects.
include main/CMakeFiles/cachebench.dir/flags.make

main/CMakeFiles/cachebench.dir/cachebench.cpp.o: main/CMakeFiles/cachebench.dir/flags.make
main/CMakeFiles/cachebench.dir/cachebench.cpp.o: /home/ucal/Mount/fdot/esesc/main/cachebench.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object main/CMakeFiles/cachebench.dir/cachebench.cpp.o"
	cd /home/ucal/Mount/fdot/build/main && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cachebench.dir/cachebench.cpp.o -c /home/ucal/Mount/fdot/esesc/main/cachebench.cpp

main/CMakeFiles/cachebench.dir/cachebench.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cachebench.dir/cachebench.cpp.i"
	cd /home/ucal/Mount/fdot/build/main && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ucal/Mount/fdot/esesc/main/cachebench.cpp > CMakeFiles/cachebench.dir/cachebench.cpp.i

main/CMakeFiles/cachebench.dir/cachebench.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cachebench.dir/cachebench.cpp.s"
	cd /home/ucal/Mount/fdot/build/main && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ucal/Mount/fdot/esesc/main/cachebench.cpp -o CMakeFiles/cachebench.dir/cachebench.cpp.s

main/CMakeFiles/cachebench.dir/cachebench.cpp.o.requires:

.PHONY : main/CMakeFiles/cachebench.dir/cachebench.cpp.o.requires

main/CMakeFiles/cachebench.dir/cachebench.cpp.o.provides: main/CMakeFiles/cachebench.dir/cachebench.cpp.o.requires
	$(MAKE) -f main/CMakeFiles/cachebench.dir/build.make main/CMakeFiles/cachebench.dir/cachebench.cpp.o.provides.build
.PHONY : main/CMakeFiles/cachebench.dir/cachebench.cpp.o.provides

main/CMakeFiles/cachebench.dir/cachebench.cpp.o.provides.build: main/CMakeFiles/cachebench.dir/cachebench.cpp.o


# Object files for target cachebench
cachebench_OBJECTS = \
"CMakeFiles/cachebench.dir/cachebench.cpp.o"

# External object files for target cachebench
cachebench_EXTERNAL_OBJECTS =

main/cachebench: main/CMakeFiles/cachebench.dir/cachebench.cpp.o
main/cachebench: main/CMakeFiles/cachebench.dir/build.make
main/cachebench: main/../qemu/mips64el-linux-user/exec.o
main/cachebench: main/../qemu/mips64el-linux-user/translate-all.o
main/cachebench: main/../qemu/mips64el-linux-user/cpu-exec.o
main/cachebench: main/../qemu/mips64el-linux-user/translate-common.o
main/cachebench: main/../qemu/mips64el-linux-user/cpu-exec-common.o
main/cachebench: main/../qemu/mips64el-linux-user/tcg/tcg.o
main/cachebench: main/../qemu/mips64el-linux-user/tcg/tcg-op.o
main/cachebench: main/../qemu/mips64el-linux-user/tcg/optimize.o
main/cachebench: main/../qemu/mips64el-linux-user/tcg/tcg-common.o
main/cachebench: main/../qemu/mips64el-linux-user/fpu/softfloat.o
main/cachebench: main/../qemu/mips64el-linux-user/disas.o
main/cachebench: main/../qemu/mips64el-linux-user/kvm-stub.o
main/cachebench: main/../qemu/mips64el-linux-user/gdbstub.o
main/cachebench: main/../qemu/mips64el-linux-user/thunk.o
main/cachebench: main/../qemu/mips64el-linux-user/user-exec.o
main/cachebench: main/../qemu/mips64el-linux-user/linux-user/main.o
main/cachebench: main/../qemu/mips64el-linux-user/linux-user/syscall.o
main/cachebench: main/../qemu/mips64el-linux-user/linux-user/strace.o
main/cachebench: main/../qemu/mips64el-linux-user/linux-user/mmap.o
main/cachebench: main/../qemu/mips64el-linux-user/linux-user/signal.o
main/cachebench: main/../qemu/mips64el-linux-user/linux-user/elfload.o
main/cachebench: main/../qemu/mips64el-linux-user/linux-user/linuxload.o
main/cachebench: main/../qemu/mips64el-linux-user/linux-user/uaccess.o
main/cachebench: main/../qemu/mips64el-linux-user/linux-user/uname.o
main/cachebench: main/../qemu/mips64el-linux-user/target-mips/translate.o
main/cachebench: main/../qemu/mips64el-linux-user/target-mips/dsp_helper.o
main/cachebench: main/../qemu/mips64el-linux-user/target-mips/op_helper.o
main/cachebench: main/../qemu/mips64el-linux-user/target-mips/lmi_helper.o
main/cachebench: main/../qemu/mips64el-linux-user/target-mips/helper.o
main/cachebench: main/../qemu/mips64el-linux-user/target-mips/cpu.o
main/cachebench: main/../qemu/mips64el-linux-user/target-mips/gdbstub.o
main/cachebench: main/../qemu/mips64el-linux-user/target-mips/msa_helper.o
main/cachebench: main/../qemu/mips64el-linux-user/target-mips/mips-semi.o
main/cachebench: main/../qemu/mips64el-linux-user/../qemu-log.o
main/cachebench: main/../qemu/mips64el-linux-user/../tcg-runtime.o
main/cachebench: main/../qemu/mips64el-linux-user/../disas/i386.o
main/cachebench: main/../qemu/mips64el-linux-user/../disas/mips.o
main/cachebench: main/../qemu/mips64el-linux-user/../hw/core/qdev.o
main/cachebench: main/../qemu/mips64el-linux-user/../hw/core/qdev-properties.o
main/cachebench: main/../qemu/mips64el-linux-user/../hw/core/fw-path-provider.o
main/cachebench: main/../qemu/mips64el-linux-user/../hw/core/irq.o
main/cachebench: main/../qemu/mips64el-linux-user/../hw/core/hotplug.o
main/cachebench: main/../qemu/mips64el-linux-user/../hw/core/nmi.o
main/cachebench: main/../qemu/mips64el-linux-user/../qom/cpu.o
main/cachebench: main/../qemu/mips64el-linux-user/trace/generated-helpers.o
main/cachebench: main/../qemu/mips64el-linux-user/../qom/object.o
main/cachebench: main/../qemu/mips64el-linux-user/../qom/container.o
main/cachebench: main/../qemu/mips64el-linux-user/../qom/qom-qobject.o
main/cachebench: main/../qemu/mips64el-linux-user/../qom/object_interfaces.o
main/cachebench: main/../qemu/mips64el-linux-user/../crypto/aes.o
main/cachebench: main/../qemu/libqemuutil.a
main/cachebench: main/../qemu/libqemustub.a
main/cachebench: main/libmain.a
main/cachebench: simu/libsampler/libsampler.a
main/cachebench: simu/libmem/libmem.a
main/cachebench: simu/libnet/libnet.a
main/cachebench: simu/libcore/libcore.a
main/cachebench: pwth/libpwrmodel/libpwrmodel.a
main/cachebench: pwth/libmcpat/libmcpat.a
main/cachebench: pwth/libsesctherm/libsesctherm.a
main/cachebench: pwth/libpeq/libpeq.a
main/cachebench: emul/libqemuint/libqemuint.a
main/cachebench: emul/libemulint/libemulint.a
main/cachebench: misc/libsuc/libsuc.a
main/cachebench: /usr/lib/x86_64-linux-gnu/libm.so
main/cachebench: /usr/lib/x86_64-linux-gnu/librt.so
main/cachebench: /usr/lib/x86_64-linux-gnu/libutil.so
main/cachebench: /usr/lib/x86_64-linux-gnu/libz.so
main/cachebench: /usr/lib/x86_64-linux-gnu/libpixman-1.so
main/cachebench: /usr/lib/x86_64-linux-gnu/libm.so
main/cachebench: /usr/lib/x86_64-linux-gnu/librt.so
main/cachebench: /usr/lib/x86_64-linux-gnu/libutil.so
main/cachebench: /usr/lib/x86_64-linux-gnu/libz.so
main/cachebench: /usr/lib/x86_64-linux-gnu/libpixman-1.so
main/cachebench: main/CMakeFiles/cachebench.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable cachebench"
	cd /home/ucal/Mount/fdot/build/main && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cachebench.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
main/CMakeFiles/cachebench.dir/build: main/cachebench

.PHONY : main/CMakeFiles/cachebench.dir/build

main/CMakeFiles/cachebench.dir/requires: main/CMakeFiles/cachebench.dir/cachebench.cpp.o.requires

.PHONY : main/CMakeFiles/cachebench.dir/requires

main/CMakeFiles/cachebench.dir/clean:
	cd /home/ucal/Mount/fdot/build/main && $(CMAKE_COMMAND) -P CMakeFiles/cachebench.dir/cmake_clean.cmake
.PHONY : main/CMakeFiles/cachebench.dir/clean

main/CMakeFiles/cachebench.dir/depend:
	cd /home/ucal/Mount/fdot/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ucal/Mount/fdot/esesc /home/ucal/Mount/fdot/esesc/main /home/ucal/Mount/fdot/build /home/ucal/Mount/fdot/build/main /home/ucal/Mount/fdot/build/main/CMakeFiles/cachebench.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : main/CMakeFiles/cachebench.dir/depend

