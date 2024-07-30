# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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
CMAKE_SOURCE_DIR = /home/tony/git/shmup

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tony/git/shmup

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/tony/git/shmup/CMakeFiles /home/tony/git/shmup//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/tony/git/shmup/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named shmup

# Build rule for target.
shmup: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 shmup
.PHONY : shmup

# fast build rule for target.
shmup/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/build
.PHONY : shmup/fast

main.o: main.c.o
.PHONY : main.o

# target to build an object file
main.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/main.c.o
.PHONY : main.c.o

main.i: main.c.i
.PHONY : main.i

# target to preprocess a source file
main.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/main.c.i
.PHONY : main.c.i

main.s: main.c.s
.PHONY : main.s

# target to generate assembly for a file
main.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/main.c.s
.PHONY : main.c.s

map.o: map.c.o
.PHONY : map.o

# target to build an object file
map.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/map.c.o
.PHONY : map.c.o

map.i: map.c.i
.PHONY : map.i

# target to preprocess a source file
map.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/map.c.i
.PHONY : map.c.i

map.s: map.c.s
.PHONY : map.s

# target to generate assembly for a file
map.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/map.c.s
.PHONY : map.c.s

sprites.o: sprites.c.o
.PHONY : sprites.o

# target to build an object file
sprites.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/sprites.c.o
.PHONY : sprites.c.o

sprites.i: sprites.c.i
.PHONY : sprites.i

# target to preprocess a source file
sprites.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/sprites.c.i
.PHONY : sprites.c.i

sprites.s: sprites.c.s
.PHONY : sprites.s

# target to generate assembly for a file
sprites.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/sprites.c.s
.PHONY : sprites.c.s

strdup.o: strdup.c.o
.PHONY : strdup.o

# target to build an object file
strdup.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/strdup.c.o
.PHONY : strdup.c.o

strdup.i: strdup.c.i
.PHONY : strdup.i

# target to preprocess a source file
strdup.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/strdup.c.i
.PHONY : strdup.c.i

strdup.s: strdup.c.s
.PHONY : strdup.s

# target to generate assembly for a file
strdup.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/strdup.c.s
.PHONY : strdup.c.s

tiles.o: tiles.c.o
.PHONY : tiles.o

# target to build an object file
tiles.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/tiles.c.o
.PHONY : tiles.c.o

tiles.i: tiles.c.i
.PHONY : tiles.i

# target to preprocess a source file
tiles.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/tiles.c.i
.PHONY : tiles.c.i

tiles.s: tiles.c.s
.PHONY : tiles.s

# target to generate assembly for a file
tiles.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/shmup.dir/build.make CMakeFiles/shmup.dir/tiles.c.s
.PHONY : tiles.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... shmup"
	@echo "... main.o"
	@echo "... main.i"
	@echo "... main.s"
	@echo "... map.o"
	@echo "... map.i"
	@echo "... map.s"
	@echo "... sprites.o"
	@echo "... sprites.i"
	@echo "... sprites.s"
	@echo "... strdup.o"
	@echo "... strdup.i"
	@echo "... strdup.s"
	@echo "... tiles.o"
	@echo "... tiles.i"
	@echo "... tiles.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

