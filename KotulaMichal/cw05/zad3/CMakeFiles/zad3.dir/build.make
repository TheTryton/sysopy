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
CMAKE_SOURCE_DIR = /home/michal/Desktop/operating_systems/KotulaMichal/cw05/zad3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/michal/Desktop/operating_systems/KotulaMichal/cw05/zad3

# Include any dependencies generated for this target.
include CMakeFiles/zad3.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/zad3.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/zad3.dir/flags.make

CMakeFiles/zad3.dir/src/main.c.o: CMakeFiles/zad3.dir/flags.make
CMakeFiles/zad3.dir/src/main.c.o: src/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/michal/Desktop/operating_systems/KotulaMichal/cw05/zad3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/zad3.dir/src/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/zad3.dir/src/main.c.o   -c /home/michal/Desktop/operating_systems/KotulaMichal/cw05/zad3/src/main.c

CMakeFiles/zad3.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zad3.dir/src/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/michal/Desktop/operating_systems/KotulaMichal/cw05/zad3/src/main.c > CMakeFiles/zad3.dir/src/main.c.i

CMakeFiles/zad3.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zad3.dir/src/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/michal/Desktop/operating_systems/KotulaMichal/cw05/zad3/src/main.c -o CMakeFiles/zad3.dir/src/main.c.s

CMakeFiles/zad3.dir/src/main.c.o.requires:

.PHONY : CMakeFiles/zad3.dir/src/main.c.o.requires

CMakeFiles/zad3.dir/src/main.c.o.provides: CMakeFiles/zad3.dir/src/main.c.o.requires
	$(MAKE) -f CMakeFiles/zad3.dir/build.make CMakeFiles/zad3.dir/src/main.c.o.provides.build
.PHONY : CMakeFiles/zad3.dir/src/main.c.o.provides

CMakeFiles/zad3.dir/src/main.c.o.provides.build: CMakeFiles/zad3.dir/src/main.c.o


# Object files for target zad3
zad3_OBJECTS = \
"CMakeFiles/zad3.dir/src/main.c.o"

# External object files for target zad3
zad3_EXTERNAL_OBJECTS =

bin/zad3: CMakeFiles/zad3.dir/src/main.c.o
bin/zad3: CMakeFiles/zad3.dir/build.make
bin/zad3: CMakeFiles/zad3.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/michal/Desktop/operating_systems/KotulaMichal/cw05/zad3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable bin/zad3"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/zad3.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/zad3.dir/build: bin/zad3

.PHONY : CMakeFiles/zad3.dir/build

CMakeFiles/zad3.dir/requires: CMakeFiles/zad3.dir/src/main.c.o.requires

.PHONY : CMakeFiles/zad3.dir/requires

CMakeFiles/zad3.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/zad3.dir/cmake_clean.cmake
.PHONY : CMakeFiles/zad3.dir/clean

CMakeFiles/zad3.dir/depend:
	cd /home/michal/Desktop/operating_systems/KotulaMichal/cw05/zad3 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/michal/Desktop/operating_systems/KotulaMichal/cw05/zad3 /home/michal/Desktop/operating_systems/KotulaMichal/cw05/zad3 /home/michal/Desktop/operating_systems/KotulaMichal/cw05/zad3 /home/michal/Desktop/operating_systems/KotulaMichal/cw05/zad3 /home/michal/Desktop/operating_systems/KotulaMichal/cw05/zad3/CMakeFiles/zad3.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/zad3.dir/depend

