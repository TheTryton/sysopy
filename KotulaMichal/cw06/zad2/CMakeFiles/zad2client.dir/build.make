# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad2

# Include any dependencies generated for this target.
include CMakeFiles/zad2client.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/zad2client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/zad2client.dir/flags.make

CMakeFiles/zad2client.dir/src/client.c.o: CMakeFiles/zad2client.dir/flags.make
CMakeFiles/zad2client.dir/src/client.c.o: src/client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/zad2client.dir/src/client.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/zad2client.dir/src/client.c.o   -c /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad2/src/client.c

CMakeFiles/zad2client.dir/src/client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zad2client.dir/src/client.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad2/src/client.c > CMakeFiles/zad2client.dir/src/client.c.i

CMakeFiles/zad2client.dir/src/client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zad2client.dir/src/client.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad2/src/client.c -o CMakeFiles/zad2client.dir/src/client.c.s

CMakeFiles/zad2client.dir/src/client.c.o.requires:

.PHONY : CMakeFiles/zad2client.dir/src/client.c.o.requires

CMakeFiles/zad2client.dir/src/client.c.o.provides: CMakeFiles/zad2client.dir/src/client.c.o.requires
	$(MAKE) -f CMakeFiles/zad2client.dir/build.make CMakeFiles/zad2client.dir/src/client.c.o.provides.build
.PHONY : CMakeFiles/zad2client.dir/src/client.c.o.provides

CMakeFiles/zad2client.dir/src/client.c.o.provides.build: CMakeFiles/zad2client.dir/src/client.c.o


# Object files for target zad2client
zad2client_OBJECTS = \
"CMakeFiles/zad2client.dir/src/client.c.o"

# External object files for target zad2client
zad2client_EXTERNAL_OBJECTS =

bin/zad2client: CMakeFiles/zad2client.dir/src/client.c.o
bin/zad2client: CMakeFiles/zad2client.dir/build.make
bin/zad2client: CMakeFiles/zad2client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable bin/zad2client"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/zad2client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/zad2client.dir/build: bin/zad2client

.PHONY : CMakeFiles/zad2client.dir/build

CMakeFiles/zad2client.dir/requires: CMakeFiles/zad2client.dir/src/client.c.o.requires

.PHONY : CMakeFiles/zad2client.dir/requires

CMakeFiles/zad2client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/zad2client.dir/cmake_clean.cmake
.PHONY : CMakeFiles/zad2client.dir/clean

CMakeFiles/zad2client.dir/depend:
	cd /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad2 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad2 /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad2 /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad2 /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad2 /home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad2/CMakeFiles/zad2client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/zad2client.dir/depend

