# BASIC PROJECT SETTINGS
cmake_minimum_required(VERSION 3.5)
project(c-cnc C)
set(CMAKE_BUILD_TYPE "Debug")

# This creates a new "machine" library
include(Components.cmake)

# VARIABLES AND PATHS DEFINITIONS
# Note: ${CMAKE_HOME_DIRECTORY} is the root project folder
#       in CMake, variables are set this way: set(VARIABLE_NAME "var_value")
#                 and are read this way: ${VARIABLE_NAME}
# Source path:
set(SOURCE_DIR ${CMAKE_HOME_DIRECTORY}/src)
# List of all files in src with name ending in '.c':
file(GLOB LIB_SOURCES ${SOURCE_DIR}/*.c)

include_directories(
  ${CMAKE_HOME_DIRECTORY}/machine/src
)

# TARGETS
# static library containing everything in src
#     first argument is the name of the target
#     second argument is the type of the library (STATIC of SHARED)
#     third argument is a list of source files
add_library(ccnclib STATIC ${LIB_SOURCES})
target_link_libraries(ccnclib m)

add_executable(test_1 ${SOURCE_DIR}/main/test.c)
target_link_libraries(test_1 ccnclib machine)

add_executable(ccnc ${SOURCE_DIR}/main/ccnc.c)
target_link_libraries(ccnc ccnclib machine)

add_executable(commissioning ${SOURCE_DIR}/main/commissioning.c)
target_link_libraries(commissioning machine)

# INSTALL
# First, set the base install folder: everything will be installed below this:
set(CMAKE_INSTALL_PREFIX ${CMAKE_HOME_DIRECTORY})
# Then define subfolders under CMAKE_INSTALL_PREFIX where to put executables
#      and libraries
install(TARGETS test_1 ccnc ccnclib commissioning
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
