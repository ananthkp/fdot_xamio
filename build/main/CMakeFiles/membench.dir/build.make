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
include main/CMakeFiles/membench.dir/depend.make

# Include the progress variables for this target.
include main/CMakeFiles/membench.dir/progress.make

# Include the compile flags for this target's objects.
include main/CMakeFiles/membench.dir/flags.make

main/CMakeFiles/membench.dir/membench.cpp.o: main/CMakeFiles/membench.dir/flags.make
main/CMakeFiles/membench.dir/membench.cpp.o: /home/ucal/Mount/fdot/esesc/main/membench.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object main/CMakeFiles/membench.dir/membench.cpp.o"
	cd /home/ucal/Mount/fdot/build/main && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/membench.dir/membench.cpp.o -c /home/ucal/Mount/fdot/esesc/main/membench.cpp

main/CMakeFiles/membench.dir/membench.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/membench.dir/membench.cpp.i"
	cd /home/ucal/Mount/fdot/build/main && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ucal/Mount/fdot/esesc/main/membench.cpp > CMakeFiles/membench.dir/membench.cpp.i

main/CMakeFiles/membench.dir/membench.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/membench.dir/membench.cpp.s"
	cd /home/ucal/Mount/fdot/build/main && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ucal/Mount/fdot/esesc/main/membench.cpp -o CMakeFiles/membench.dir/membench.cpp.s

main/CMakeFiles/membench.dir/membench.cpp.o.requires:

.PHONY : main/CMakeFiles/membench.dir/membench.cpp.o.requires

main/CMakeFiles/membench.dir/membench.cpp.o.provides: main/CMakeFiles/membench.dir/membench.cpp.o.requires
	$(MAKE) -f main/CMakeFiles/membench.dir/build.make main/CMakeFiles/membench.dir/membench.cpp.o.provides.build
.PHONY : main/CMakeFiles/membench.dir/membench.cpp.o.provides

main/CMakeFiles/membench.dir/membench.cpp.o.provides.build: main/CMakeFiles/membench.dir/membench.cpp.o


# Object files for target membench
membench_OBJECTS = \
"CMakeFiles/membench.dir/membench.cpp.o"

# External object files for target membench
membench_EXTERNAL_OBJECTS =

main/membench: main/CMakeFiles/membench.dir/membench.cpp.o
main/membench: main/CMakeFiles/membench.dir/build.make
main/membench: main/../qemu/mips64el-linux-user/exec.o
main/membench: main/../qemu/mips64el-linux-user/translate-all.o
main/membench: main/../qemu/mips64el-linux-user/cpu-exec.o
main/membench: main/../qemu/mips64el-linux-user/translate-common.o
main/membench: main/../qemu/mips64el-linux-user/cpu-exec-common.o
main/membench: main/../qemu/mips64el-linux-user/tcg/tcg.o
main/membench: main/../qemu/mips64el-linux-user/tcg/tcg-op.o
main/membench: main/../qemu/mips64el-linux-user/tcg/optimize.o
main/membench: main/../qemu/mips64el-linux-user/tcg/tcg-common.o
main/membench: main/../qemu/mips64el-linux-user/fpu/softfloat.o
main/membench: main/../qemu/mips64el-linux-user/disas.o
main/membench: main/../qemu/mips64el-linux-user/kvm-stub.o
main/membench: main/../qemu/mips64el-linux-user/gdbstub.o
main/membench: main/../qemu/mips64el-linux-user/thunk.o
main/membench: main/../qemu/mips64el-linux-user/user-exec.o
main/membench: main/../qemu/mips64el-linux-user/linux-user/main.o
main/membench: main/../qemu/mips64el-linux-user/linux-user/syscall.o
main/membench: main/../qemu/mips64el-linux-user/linux-user/strace.o
main/membench: main/../qemu/mips64el-linux-user/linux-user/mmap.o
main/membench: main/../qemu/mips64el-linux-user/linux-user/signal.o
main/membench: main/../qemu/mips64el-linux-user/linux-user/elfload.o
main/membench: main/../qemu/mips64el-linux-user/linux-user/linuxload.o
main/membench: main/../qemu/mips64el-linux-user/linux-user/uaccess.o
main/membench: main/../qemu/mips64el-linux-user/linux-user/uname.o
main/membench: main/../qemu/mips64el-linux-user/target-mips/translate.o
main/membench: main/../qemu/mips64el-linux-user/target-mips/dsp_helper.o
main/membench: main/../qemu/mips64el-linux-user/target-mips/op_helper.o
main/membench: main/../qemu/mips64el-linux-user/target-mips/lmi_helper.o
main/membench: main/../qemu/mips64el-linux-user/target-mips/helper.o
main/membench: main/../qemu/mips64el-linux-user/target-mips/cpu.o
main/membench: main/../qemu/mips64el-linux-user/target-mips/gdbstub.o
main/membench: main/../qemu/mips64el-linux-user/target-mips/msa_helper.o
main/membench: main/../qemu/mips64el-linux-user/target-mips/mips-semi.o
main/membench: main/../qemu/mips64el-linux-user/../qemu-log.o
main/membench: main/../qemu/mips64el-linux-user/../tcg-runtime.o
main/membench: main/../qemu/mips64el-linux-user/../disas/i386.o
main/membench: main/../qemu/mips64el-linux-user/../disas/mips.o
main/membench: main/../qemu/mips64el-linux-user/../hw/core/qdev.o
main/membench: main/../qemu/mips64el-linux-user/../hw/core/qdev-properties.o
main/membench: main/../qemu/mips64el-linux-user/../hw/core/fw-path-provider.o
main/membench: main/../qemu/mips64el-linux-user/../hw/core/irq.o
main/membench: main/../qemu/mips64el-linux-user/../hw/core/hotplug.o
main/membench: main/../qemu/mips64el-linux-user/../hw/core/nmi.o
main/membench: main/../qemu/mips64el-linux-user/../qom/cpu.o
main/membench: main/../qemu/mips64el-linux-user/trace/generated-helpers.o
main/membench: main/../qemu/mips64el-linux-user/../qom/object.o
main/membench: main/../qemu/mips64el-linux-user/../qom/container.o
main/membench: main/../qemu/mips64el-linux-user/../qom/qom-qobject.o
main/membench: main/../qemu/mips64el-linux-user/../qom/object_interfaces.o
main/membench: main/../qemu/mips64el-linux-user/../crypto/aes.o
main/membench: main/../qemu/libqemuutil.a
main/membench: main/../qemu/libqemustub.a
main/membench: main/libmain.a
main/membench: simu/libsampler/libsampler.a
main/membench: simu/libmem/libmem.a
main/membench: simu/libnet/libnet.a
main/membench: simu/libcore/libcore.a
main/membench: pwth/libpwrmodel/libpwrmodel.a
main/membench: pwth/libmcpat/libmcpat.a
main/membench: pwth/libsesctherm/libsesctherm.a
main/membench: pwth/libpeq/libpeq.a
main/membench: emul/libqemuint/libqemuint.a
main/membench: emul/libemulint/libemulint.a
main/membench: misc/libsuc/libsuc.a
main/membench: /usr/lib/x86_64-linux-gnu/libm.so
main/membench: /usr/lib/x86_64-linux-gnu/librt.so
main/membench: /usr/lib/x86_64-linux-gnu/libutil.so
main/membench: /usr/lib/x86_64-linux-gnu/libz.so
main/membench: /usr/lib/x86_64-linux-gnu/libpixman-1.so
main/membench: /usr/lib/x86_64-linux-gnu/libm.so
main/membench: /usr/lib/x86_64-linux-gnu/librt.so
main/membench: /usr/lib/x86_64-linux-gnu/libutil.so
main/membench: /usr/lib/x86_64-linux-gnu/libz.so
main/membench: /usr/lib/x86_64-linux-gnu/libpixman-1.so
main/membench: main/CMakeFiles/membench.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ucal/Mount/fdot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable membench"
	cd /home/ucal/Mount/fdot/build/main && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/membench.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
main/CMakeFiles/membench.dir/build: main/membench

.PHONY : main/CMakeFiles/membench.dir/build

main/CMakeFiles/membench.dir/requires: main/CMakeFiles/membench.dir/membench.cpp.o.requires

.PHONY : main/CMakeFiles/membench.dir/requires

main/CMakeFiles/membench.dir/clean:
	cd /home/ucal/Mount/fdot/build/main && $(CMAKE_COMMAND) -P CMakeFiles/membench.dir/cmake_clean.cmake
.PHONY : main/CMakeFiles/membench.dir/clean

main/CMakeFiles/membench.dir/depend:
	cd /home/ucal/Mount/fdot/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ucal/Mount/fdot/esesc /home/ucal/Mount/fdot/esesc/main /home/ucal/Mount/fdot/build /home/ucal/Mount/fdot/build/main /home/ucal/Mount/fdot/build/main/CMakeFiles/membench.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : main/CMakeFiles/membench.dir/depend

