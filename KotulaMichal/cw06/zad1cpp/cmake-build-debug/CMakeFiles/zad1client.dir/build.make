# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /snap/clion/110/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/110/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/zad1client.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/zad1client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/zad1client.dir/flags.make

CMakeFiles/zad1client.dir/src/client.cpp.o: CMakeFiles/zad1client.dir/flags.make
CMakeFiles/zad1client.dir/src/client.cpp.o: ../src/client.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/zad1client.dir/src/client.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/zad1client.dir/src/client.cpp.o -c /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1/src/client.cpp

CMakeFiles/zad1client.dir/src/client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/zad1client.dir/src/client.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1/src/client.cpp > CMakeFiles/zad1client.dir/src/client.cpp.i

CMakeFiles/zad1client.dir/src/client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/zad1client.dir/src/client.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1/src/client.cpp -o CMakeFiles/zad1client.dir/src/client.cpp.s

# Object files for target zad1client
zad1client_OBJECTS = \
"CMakeFiles/zad1client.dir/src/client.cpp.o"

# External object files for target zad1client
zad1client_EXTERNAL_OBJECTS =

../bin/zad1client: CMakeFiles/zad1client.dir/src/client.cpp.o
../bin/zad1client: CMakeFiles/zad1client.dir/build.make
../bin/zad1client: ../bin/libcommon.a
../bin/zad1client: CMakeFiles/zad1client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/zad1client"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/zad1client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/zad1client.dir/build: ../bin/zad1client

.PHONY : CMakeFiles/zad1client.dir/build

CMakeFiles/zad1client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/zad1client.dir/cmake_clean.cmake
.PHONY : CMakeFiles/zad1client.dir/clean

CMakeFiles/zad1client.dir/depend:
	cd /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1 /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1 /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1/cmake-build-debug /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1/cmake-build-debug /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1/cmake-build-debug/CMakeFiles/zad1client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/zad1client.dir/depend
