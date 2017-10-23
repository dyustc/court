cmake_minimum_required (VERSION 2.6)
project (Tutorial)
# The version number
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)
set ( PROJECT_SOURCE_DIR /home/yzdai/gw/cmake_tutorial)
set ( PROJECT_BINARY_DIR /home/yzdai/gw/cmake_tutorial/bin )

# should we use our own math functions?
option (USE_MYMATH
        "Use tutorial provided math implementation" ON)

# config a header file to pass some of the CMake settings
# to the source code
configure_file (
    "${PROJECT_SOURCE_DIR}/TutorialConfig.h.in"
    "${PROJECT_BINARY_DIR}/TutorialConfig.h"
    )

# add the binary tree to the search path for include files
# so that we will find TutorialCongig.h
include_directories("${PROJECT_BINARY_DIR}")

# add the MathFunctions library?
#
if (USE_MYMATH)
    include_directories("${PROJECT_SOURCE_DIR}/MathFunctions")
    add_subdirectory (${PROJECT_SOURCE_DIR}/MathFunctions)
    set (EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)
endif (USE_MYMATH)

# add the executable
add_executable (Tutorial ${PROJECT_BINARY_DIR}/tutorial.c)
target_link_libraries (Tutorial -lm)
target_link_libraries (Tutorial ${EXTRA_LIBS})

# add the install targets
install (TARGETS Tutorial DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/TutorialConfig.h" DESTINATION include)

include (CTest)

# does the application run
add_test (TutorialRun Tutorial 25)

# does it sqrt of 25
add_test (TutorialComp25 Tutorial 25)
set_tests_properties (TutorialComp25 PROPERITES PASS_REGULAR_EXPRESSION "25 is 5")

# does it handle negative numbers
add_test (TutorialNegative Tutorial -25)
set_tests_properties (TutorialNegative PROPERITES PASS_REGULAR_EXPRESSION "-25 is 0")

# does it handle small numbers
add_test (TutorialSmall Tutorial 0.0001)
set_tests_properties (TutorialSmall PROPERITES PASS_REGULAR_EXPRESSION "0.0001 is 0.01")

# does the usage message work?
add_test (TutorialUsage Tutorial)
set_tests_properties (TutorialUsage PROPERITES PASS_REGULAR_EXPRESSION "Usage:.*number")

