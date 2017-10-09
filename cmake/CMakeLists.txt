cmake_minimum_required (VERSION 2.6)
project (Tutorial)
# The version number
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)
set ( ENV{PROJECT_BINARY_DIR} /home/yzdai/test_for_git/cmake )
# set ( ENV{PROJECT_SOURCE_DIR} /home/yzdai/test_for_git/cmake )

# config a header file to pass some of the CMake settings
# to the source code
configure_file (
    "/home/yzdai/test_for_git/cmake/TutorialConfig.h.in"
    "/home/yzdai/test_for_git/cmake/TutorialConfig.h"
    )

# add the binary tree to the search path for include files
# so that we will find TutorialCongig.h
include_directories("$(PROJECT_BINARY_DIR)")

# add the executable
add_executable (Tutorial tutorial.c)
target_link_libraries (Tutorial -lm)
