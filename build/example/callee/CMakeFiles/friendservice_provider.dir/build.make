# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/melody/myrpc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/melody/myrpc/build

# Include any dependencies generated for this target.
include example/callee/CMakeFiles/friendservice_provider.dir/depend.make

# Include the progress variables for this target.
include example/callee/CMakeFiles/friendservice_provider.dir/progress.make

# Include the compile flags for this target's objects.
include example/callee/CMakeFiles/friendservice_provider.dir/flags.make

example/callee/CMakeFiles/friendservice_provider.dir/friendservice.cc.o: example/callee/CMakeFiles/friendservice_provider.dir/flags.make
example/callee/CMakeFiles/friendservice_provider.dir/friendservice.cc.o: ../example/callee/friendservice.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/melody/myrpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object example/callee/CMakeFiles/friendservice_provider.dir/friendservice.cc.o"
	cd /home/melody/myrpc/build/example/callee && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/friendservice_provider.dir/friendservice.cc.o -c /home/melody/myrpc/example/callee/friendservice.cc

example/callee/CMakeFiles/friendservice_provider.dir/friendservice.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/friendservice_provider.dir/friendservice.cc.i"
	cd /home/melody/myrpc/build/example/callee && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/melody/myrpc/example/callee/friendservice.cc > CMakeFiles/friendservice_provider.dir/friendservice.cc.i

example/callee/CMakeFiles/friendservice_provider.dir/friendservice.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/friendservice_provider.dir/friendservice.cc.s"
	cd /home/melody/myrpc/build/example/callee && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/melody/myrpc/example/callee/friendservice.cc -o CMakeFiles/friendservice_provider.dir/friendservice.cc.s

example/callee/CMakeFiles/friendservice_provider.dir/__/friend.pb.cc.o: example/callee/CMakeFiles/friendservice_provider.dir/flags.make
example/callee/CMakeFiles/friendservice_provider.dir/__/friend.pb.cc.o: ../example/friend.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/melody/myrpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object example/callee/CMakeFiles/friendservice_provider.dir/__/friend.pb.cc.o"
	cd /home/melody/myrpc/build/example/callee && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/friendservice_provider.dir/__/friend.pb.cc.o -c /home/melody/myrpc/example/friend.pb.cc

example/callee/CMakeFiles/friendservice_provider.dir/__/friend.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/friendservice_provider.dir/__/friend.pb.cc.i"
	cd /home/melody/myrpc/build/example/callee && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/melody/myrpc/example/friend.pb.cc > CMakeFiles/friendservice_provider.dir/__/friend.pb.cc.i

example/callee/CMakeFiles/friendservice_provider.dir/__/friend.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/friendservice_provider.dir/__/friend.pb.cc.s"
	cd /home/melody/myrpc/build/example/callee && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/melody/myrpc/example/friend.pb.cc -o CMakeFiles/friendservice_provider.dir/__/friend.pb.cc.s

# Object files for target friendservice_provider
friendservice_provider_OBJECTS = \
"CMakeFiles/friendservice_provider.dir/friendservice.cc.o" \
"CMakeFiles/friendservice_provider.dir/__/friend.pb.cc.o"

# External object files for target friendservice_provider
friendservice_provider_EXTERNAL_OBJECTS =

../bin/friendservice_provider: example/callee/CMakeFiles/friendservice_provider.dir/friendservice.cc.o
../bin/friendservice_provider: example/callee/CMakeFiles/friendservice_provider.dir/__/friend.pb.cc.o
../bin/friendservice_provider: example/callee/CMakeFiles/friendservice_provider.dir/build.make
../bin/friendservice_provider: ../lib/libmprpc.a
../bin/friendservice_provider: /usr/local/lib/libprotobuf.so
../bin/friendservice_provider: example/callee/CMakeFiles/friendservice_provider.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/melody/myrpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../../../bin/friendservice_provider"
	cd /home/melody/myrpc/build/example/callee && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/friendservice_provider.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
example/callee/CMakeFiles/friendservice_provider.dir/build: ../bin/friendservice_provider

.PHONY : example/callee/CMakeFiles/friendservice_provider.dir/build

example/callee/CMakeFiles/friendservice_provider.dir/clean:
	cd /home/melody/myrpc/build/example/callee && $(CMAKE_COMMAND) -P CMakeFiles/friendservice_provider.dir/cmake_clean.cmake
.PHONY : example/callee/CMakeFiles/friendservice_provider.dir/clean

example/callee/CMakeFiles/friendservice_provider.dir/depend:
	cd /home/melody/myrpc/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/melody/myrpc /home/melody/myrpc/example/callee /home/melody/myrpc/build /home/melody/myrpc/build/example/callee /home/melody/myrpc/build/example/callee/CMakeFiles/friendservice_provider.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : example/callee/CMakeFiles/friendservice_provider.dir/depend

