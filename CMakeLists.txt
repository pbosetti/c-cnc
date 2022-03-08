cmake_minimum_required(VERSION 3.10)

#   _____          _ _           _                        
#  |  __ \        | (_)         (_)                       
#  | |__) | __ ___| |_ _ __ ___  _ _ __   __ _ _ __ _   _ 
#  |  ___/ '__/ _ \ | | '_ ` _ \| | '_ \ / _` | '__| | | |
#  | |   | | |  __/ | | | | | | | | | | | (_| | |  | |_| |
#  |_|   |_|  \___|_|_|_| |_| |_|_|_| |_|\__,_|_|   \__, |
#                                                    __/ |
#                                                   |___/ 
set(VERSION "1.0")
project(C-CNC VERSION ${VERSION} LANGUAGES C CXX)

#    _____                           _____                      
#   / ____|                         / ____|                     
#  | |     _ __ ___  ___ ___ ______| |     ___  _ __ ___  _ __  
#  | |    | '__/ _ \/ __/ __|______| |    / _ \| '_ ` _ \| '_ \ 
#  | |____| | | (_) \__ \__ \      | |___| (_) | | | | | | |_) |
#   \_____|_|  \___/|___/___/       \_____\___/|_| |_| |_| .__/ 
#                                                        | |    
#                                                        |_|    
# Find out target platform
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpmachine OUTPUT_VARIABLE TARGET_PLATFORM) #put into TARGET_PLATFORM result of $ gcc -dumpmachine
string(REPLACE "\n" "" TARGET_PLATFORM ${TARGET_PLATFORM})
string(REGEX MATCH mipsel-openwrt-linux-musl IS_MIPSEL ${TARGET_PLATFORM})
string(REGEX MATCH armv7-unknown-linux-gnueabi IS_ARM7 ${TARGET_PLATFORM})
string(REGEX MATCH arm-linux-gnueabihf IS_ARM6 ${TARGET_PLATFORM})
string(REGEX MATCH arm-cortexa8_neon-linux-gnueabihf IS_ARM7A ${TARGET_PLATFORM})
string(REGEX MATCH aarch64-unknown-linux-gnueabi IS_ARM64 ${TARGET_PLATFORM})
if (IS_ARM6 OR IS_ARM7 OR IS_ARM7A OR IS_ARM64)
  set(IS_ARM TRUE)
endif()

# SETUP PATHS AND VARIABLES
if(IS_MIPSEL)
  message(STATUS "Cross-building for MIPSEL (cross_root: ${cross_root})")
  add_definitions(-D_OPENWRT_LINUX=1)
  include_directories(${cross_root}/include)
  link_directories(${cross_root}/lib)
elseif(IS_ARM)
  message(STATUS "Cross-building for ARM (cross_root: ${cross_root})")
  set(LINUX TRUE)
  include_directories(${cross_root}/include)
  link_directories(${cross_root}/lib)
  add_definitions(-D__ARM7__)
elseif(UNIX AND NOT APPLE)
  message(STATUS "Building on Linux")
  set(LINUX TRUE)
  set(NATIVE TRUE)
  include_directories(/usr/local/include)
  link_directories(/usr/lib/x86_64-linux-gnu)
elseif(APPLE)
  message(STATUS "Building on Apple OS X")
  set(NATIVE TRUE)
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
endif()

if(NOT CMAKE_BUILD_TYPE)
  if(NATIVE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Native targets default to Debug" FORCE)
  else()
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Cross-build default to Release" FORCE)
  endif()
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

# COMPILE OPTIONS
add_compile_options(-fPIC -D_GNU_SOURCE)
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  message(STATUS "Debug mode, enabling all warnings")
  add_compile_options(-Wall -Wno-comment)
endif()

# RPATH management
if (APPLE)
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
else()
  set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
endif()

#   _____           _           _     _____       _        _ _     
#  |  __ \         (_)         | |   |  __ \     | |      (_) |    
#  | |__) | __ ___  _  ___  ___| |_  | |  | | ___| |_ __ _ _| |___ 
#  |  ___/ '__/ _ \| |/ _ \/ __| __| | |  | |/ _ \ __/ _` | | / __|
#  | |   | | | (_) | |  __/ (__| |_  | |__| |  __/ || (_| | | \__ \
#  |_|   |_|  \___/| |\___|\___|\__| |_____/ \___|\__\__,_|_|_|___/
#                 _/ |                                             
#                |__/                                              
# FILES
# expect all sources in /src, except mains
set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
# file(GLOB LIB_SOURCES "${SOURCE_DIR}/*.c")
file(GLOB LIB_SOURCES_CPP "${SOURCE_DIR}/*.cpp")
set_source_files_properties(${LIB_SOURCES_CPP} PROPERTIES COMPILE_OPTIONS -std=c++14)
file(GLOB LIB_SOURCES "${SOURCE_DIR}/*.c")
set_source_files_properties(${LIB_SOURCES} PROPERTIES COMPILE_OPTIONS -std=c11)
# generate defines.h, which also contains version numbers matching git tags
configure_file(
  ${SOURCE_DIR}/defines.h.in
  ${SOURCE_DIR}/defines.h
)

# TARGETS
add_executable(ini_test ${SOURCE_DIR}/main/ini_test.c)

list(APPEND TARGETS_LIST
  ini_test
)

if(IS_ARM)
  add_library(${PROJECT_NAME}_static STATIC ${LIB_SOURCES} ${LIB_SOURCES_CPP})
  target_link_libraries(ini_test ${PROJECT_NAME}_static)
else()
  add_library(${PROJECT_NAME}_shared SHARED ${LIB_SOURCES} ${LIB_SOURCES_CPP})
  target_link_libraries(ini_test ${PROJECT_NAME}_shared)
  list(APPEND TARGETS_LIST ${PROJECT_NAME}_shared)
endif()

#   _____           _        _ _ 
#  |_   _|         | |      | | |
#    | |  _ __  ___| |_ __ _| | |
#    | | | '_ \/ __| __/ _` | | |
#   _| |_| | | \__ \ || (_| | | |
#  |_____|_| |_|___/\__\__,_|_|_|
if(NATIVE)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/products_host)
else()
  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/products)
endif()
install(TARGETS ${TARGETS_LIST}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib/static
)

#   _____           _               _             
#  |  __ \         | |             (_)            
#  | |__) |_ _  ___| | ____ _  __ _ _ _ __   __ _ 
#  |  ___/ _` |/ __| |/ / _` |/ _` | | '_ \ / _` |
#  | |  | (_| | (__|   < (_| | (_| | | | | | (_| |
#  |_|   \__,_|\___|_|\_\__,_|\__, |_|_| |_|\__, |
#                              __/ |         __/ |
#                             |___/         |___/ 
set(CPACK_GENERATOR "STGZ")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_NAME ${PROJECT_NAME}-${CMAKE_BUILD_TYPE})
include(CPack)

#   _____             _              __ _ _      
#  |  __ \           | |            / _(_) |     
#  | |  | | ___   ___| | _____ _ __| |_ _| | ___ 
#  | |  | |/ _ \ / __| |/ / _ \ '__|  _| | |/ _ \
#  | |__| | (_) | (__|   <  __/ |  | | | | |  __/
#  |_____/ \___/ \___|_|\_\___|_|  |_| |_|_|\___|
#                                              
# if not cross-compiling, create Dockerfile for the target platform
message(STATUS "Host Dockerfile generated for target ${TARGET_NAME}")
if(NATIVE)
  option(ENABLE_MRUBY "Enable build of mruby in container" OFF)
  if(NOT ENABLE_MRUBY)
    message(NOTICE "\tmruby is NOT compiled in the container")
    set(COMMENT_ENABLE_MRUBY "#")
  endif()
  option(ENABLE_OPENBLAS "Enable build of OpenBLAS in container" OFF)
  if(NOT ENABLE_OPENBLAS)
    message(NOTICE "\tOpenBLAS is NOT compiled in the container")
    set(COMMENT_ENABLE_OPENBLAS "#")
  endif()
  option(KEEP_BUILD_DIR "Do not delete build dir in Docker container" OFF)
  if(KEEP_BUILD_DIR)
    message(NOTICE "\tBuild dir is NOT deleted after compilation")
    set(COMMENT_BUILD_DIR "#")
  endif()
  set(TARGET_NAME CACHE STRING "Select TARGET platform")
  if(NOT TARGET_NAME)
    set(TARGET_NAME "armv7a")
  endif()
  set_property(CACHE TARGET_NAME PROPERTY STRINGS mipsel armv6 armv7 armv7a arm64)
  configure_file(
    ${CMAKE_HOME_DIRECTORY}/Dockerfile.in
    ${CMAKE_HOME_DIRECTORY}/Dockerfile
    @ONLY
  )
  message(NOTICE "\tBuilding of mruby (libmruby.a) is: ${ENABLE_MRUBY}")
  message(NOTICE "\tKeep the build root directory: ${KEEP_BUILD_DIR}")
  message(NOTICE "\tBuild with: docker build -t ${TARGET_NAME} .")
  message(NOTICE "\tthen:       docker run --rm ${TARGET_NAME} > ${TARGET_NAME} && chmod a+x ${TARGET_NAME}")
endif()
