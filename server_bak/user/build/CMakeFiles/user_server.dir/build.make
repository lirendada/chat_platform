# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/liren/chat_platform/server/user

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/liren/chat_platform/server/user/build

# Include any dependencies generated for this target.
include CMakeFiles/user_server.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/user_server.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/user_server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/user_server.dir/flags.make

base.pb.cc: /home/liren/chat_platform/server/proto/base.proto
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/liren/chat_platform/server/user/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) /home/liren/chat_platform/server/user/build/base.pb.cc
	protoc --cpp_out=/home/liren/chat_platform/server/user/build -I /home/liren/chat_platform/server/user/../proto --experimental_allow_proto3_optional /home/liren/chat_platform/server/user/../proto/base.proto

user.pb.cc: /home/liren/chat_platform/server/proto/user.proto
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/liren/chat_platform/server/user/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) /home/liren/chat_platform/server/user/build/user.pb.cc
	protoc --cpp_out=/home/liren/chat_platform/server/user/build -I /home/liren/chat_platform/server/user/../proto --experimental_allow_proto3_optional /home/liren/chat_platform/server/user/../proto/user.proto

file.pb.cc: /home/liren/chat_platform/server/proto/file.proto
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/liren/chat_platform/server/user/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) /home/liren/chat_platform/server/user/build/file.pb.cc
	protoc --cpp_out=/home/liren/chat_platform/server/user/build -I /home/liren/chat_platform/server/user/../proto --experimental_allow_proto3_optional /home/liren/chat_platform/server/user/../proto/file.proto

user-odb.cxx: /home/liren/chat_platform/server/odb/user.hxx
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/liren/chat_platform/server/user/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) /home/liren/chat_platform/server/user/build/user-odb.cxx
	odb -d mysql --std c++11 --generate-query --generate-schema --profile boost/date-time /home/liren/chat_platform/server/user/../odb/user.hxx

CMakeFiles/user_server.dir/source/user_server.cc.o: CMakeFiles/user_server.dir/flags.make
CMakeFiles/user_server.dir/source/user_server.cc.o: /home/liren/chat_platform/server/user/source/user_server.cc
CMakeFiles/user_server.dir/source/user_server.cc.o: CMakeFiles/user_server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/liren/chat_platform/server/user/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/user_server.dir/source/user_server.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/user_server.dir/source/user_server.cc.o -MF CMakeFiles/user_server.dir/source/user_server.cc.o.d -o CMakeFiles/user_server.dir/source/user_server.cc.o -c /home/liren/chat_platform/server/user/source/user_server.cc

CMakeFiles/user_server.dir/source/user_server.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/user_server.dir/source/user_server.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/liren/chat_platform/server/user/source/user_server.cc > CMakeFiles/user_server.dir/source/user_server.cc.i

CMakeFiles/user_server.dir/source/user_server.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/user_server.dir/source/user_server.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/liren/chat_platform/server/user/source/user_server.cc -o CMakeFiles/user_server.dir/source/user_server.cc.s

CMakeFiles/user_server.dir/base.pb.cc.o: CMakeFiles/user_server.dir/flags.make
CMakeFiles/user_server.dir/base.pb.cc.o: base.pb.cc
CMakeFiles/user_server.dir/base.pb.cc.o: CMakeFiles/user_server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/liren/chat_platform/server/user/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/user_server.dir/base.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/user_server.dir/base.pb.cc.o -MF CMakeFiles/user_server.dir/base.pb.cc.o.d -o CMakeFiles/user_server.dir/base.pb.cc.o -c /home/liren/chat_platform/server/user/build/base.pb.cc

CMakeFiles/user_server.dir/base.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/user_server.dir/base.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/liren/chat_platform/server/user/build/base.pb.cc > CMakeFiles/user_server.dir/base.pb.cc.i

CMakeFiles/user_server.dir/base.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/user_server.dir/base.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/liren/chat_platform/server/user/build/base.pb.cc -o CMakeFiles/user_server.dir/base.pb.cc.s

CMakeFiles/user_server.dir/user.pb.cc.o: CMakeFiles/user_server.dir/flags.make
CMakeFiles/user_server.dir/user.pb.cc.o: user.pb.cc
CMakeFiles/user_server.dir/user.pb.cc.o: CMakeFiles/user_server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/liren/chat_platform/server/user/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/user_server.dir/user.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/user_server.dir/user.pb.cc.o -MF CMakeFiles/user_server.dir/user.pb.cc.o.d -o CMakeFiles/user_server.dir/user.pb.cc.o -c /home/liren/chat_platform/server/user/build/user.pb.cc

CMakeFiles/user_server.dir/user.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/user_server.dir/user.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/liren/chat_platform/server/user/build/user.pb.cc > CMakeFiles/user_server.dir/user.pb.cc.i

CMakeFiles/user_server.dir/user.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/user_server.dir/user.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/liren/chat_platform/server/user/build/user.pb.cc -o CMakeFiles/user_server.dir/user.pb.cc.s

CMakeFiles/user_server.dir/file.pb.cc.o: CMakeFiles/user_server.dir/flags.make
CMakeFiles/user_server.dir/file.pb.cc.o: file.pb.cc
CMakeFiles/user_server.dir/file.pb.cc.o: CMakeFiles/user_server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/liren/chat_platform/server/user/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/user_server.dir/file.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/user_server.dir/file.pb.cc.o -MF CMakeFiles/user_server.dir/file.pb.cc.o.d -o CMakeFiles/user_server.dir/file.pb.cc.o -c /home/liren/chat_platform/server/user/build/file.pb.cc

CMakeFiles/user_server.dir/file.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/user_server.dir/file.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/liren/chat_platform/server/user/build/file.pb.cc > CMakeFiles/user_server.dir/file.pb.cc.i

CMakeFiles/user_server.dir/file.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/user_server.dir/file.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/liren/chat_platform/server/user/build/file.pb.cc -o CMakeFiles/user_server.dir/file.pb.cc.s

CMakeFiles/user_server.dir/user-odb.cxx.o: CMakeFiles/user_server.dir/flags.make
CMakeFiles/user_server.dir/user-odb.cxx.o: user-odb.cxx
CMakeFiles/user_server.dir/user-odb.cxx.o: CMakeFiles/user_server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/liren/chat_platform/server/user/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/user_server.dir/user-odb.cxx.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/user_server.dir/user-odb.cxx.o -MF CMakeFiles/user_server.dir/user-odb.cxx.o.d -o CMakeFiles/user_server.dir/user-odb.cxx.o -c /home/liren/chat_platform/server/user/build/user-odb.cxx

CMakeFiles/user_server.dir/user-odb.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/user_server.dir/user-odb.cxx.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/liren/chat_platform/server/user/build/user-odb.cxx > CMakeFiles/user_server.dir/user-odb.cxx.i

CMakeFiles/user_server.dir/user-odb.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/user_server.dir/user-odb.cxx.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/liren/chat_platform/server/user/build/user-odb.cxx -o CMakeFiles/user_server.dir/user-odb.cxx.s

# Object files for target user_server
user_server_OBJECTS = \
"CMakeFiles/user_server.dir/source/user_server.cc.o" \
"CMakeFiles/user_server.dir/base.pb.cc.o" \
"CMakeFiles/user_server.dir/user.pb.cc.o" \
"CMakeFiles/user_server.dir/file.pb.cc.o" \
"CMakeFiles/user_server.dir/user-odb.cxx.o"

# External object files for target user_server
user_server_EXTERNAL_OBJECTS =

user_server: CMakeFiles/user_server.dir/source/user_server.cc.o
user_server: CMakeFiles/user_server.dir/base.pb.cc.o
user_server: CMakeFiles/user_server.dir/user.pb.cc.o
user_server: CMakeFiles/user_server.dir/file.pb.cc.o
user_server: CMakeFiles/user_server.dir/user-odb.cxx.o
user_server: CMakeFiles/user_server.dir/build.make
user_server: /usr/lib/x86_64-linux-gnu/libjsoncpp.so.19
user_server: CMakeFiles/user_server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/liren/chat_platform/server/user/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking CXX executable user_server"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/user_server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/user_server.dir/build: user_server
.PHONY : CMakeFiles/user_server.dir/build

CMakeFiles/user_server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/user_server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/user_server.dir/clean

CMakeFiles/user_server.dir/depend: base.pb.cc
CMakeFiles/user_server.dir/depend: file.pb.cc
CMakeFiles/user_server.dir/depend: user-odb.cxx
CMakeFiles/user_server.dir/depend: user.pb.cc
	cd /home/liren/chat_platform/server/user/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/liren/chat_platform/server/user /home/liren/chat_platform/server/user /home/liren/chat_platform/server/user/build /home/liren/chat_platform/server/user/build /home/liren/chat_platform/server/user/build/CMakeFiles/user_server.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/user_server.dir/depend

