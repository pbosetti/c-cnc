cmake_minimum_required(VERSION 3.10)
# NOTE: This project is mostly already configured.
#       Go to the "Targets" section to add new products
#   _____          _ _           _                        
#  |  __ \        | (_)         (_)                       
#  | |__) | __ ___| |_ _ __ ___  _ _ __   __ _ _ __ _   _ 
#  |  ___/ '__/ _ \ | | '_ ` _ \| | '_ \ / _` | '__| | | |
#  | |   | | |  __/ | | | | | | | | | | | (_| | |  | |_| |
#  |_|   |_|  \___|_|_|_| |_| |_|_|_| |_|\__,_|_|   \__, |
#                                                    __/ |
#                                                   |___/ 
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed! Create a build directory and run CMake from there.")
endif()
# This is dynamically updated in defines.h.in:
set(VERSION "1.0")
project(C-CNC VERSION ${VERSION} LANGUAGES C CXX)

#  __  __     ____        _ _     _ 
#  \ \/ /    | __ ) _   _(_) | __| |
#   \  /_____|  _ \| | | | | |/ _` |
#   /  \_____| |_) | |_| | | | (_| |
#  /_/\_\    |____/ \__,_|_|_|\__,_|
# 
# Find out target platform
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpmachine OUTPUT_VARIABLE TARGET_PLATFORM) #put into TARGET_PLATFORM result of $ gcc -dumpmachine
string(REPLACE "\n" "" TARGET_PLATFORM ${TARGET_PLATFORM})
string(REGEX MATCH mipsel-openwrt-linux-musl IS_MIPSEL ${TARGET_PLATFORM})
string(REGEX MATCH armv7-unknown-linux-gnueabi IS_ARM7 ${TARGET_PLATFORM})
string(REGEX MATCH armv6-unknown-linux-gnueabihf IS_ARM6 ${TARGET_PLATFORM})
string(REGEX MATCH arm-cortexa8_neon-linux-gnueabihf IS_ARM7A ${TARGET_PLATFORM})
string(REGEX MATCH aarch64-unknown-linux-gnu IS_ARM64 ${TARGET_PLATFORM})
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
  link_directories(/usr/lib/x86_64-linux-gnu /usr/local/lib)
elseif(APPLE)
  message(STATUS "Building on Apple OS X")
  set(NATIVE TRUE)
  # On Intel Macs, brew uses /usr/local
  if(EXISTS /usr/local)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
  endif()
  # on M1 Macs, brew uses /opt/homebrew:
  if(EXISTS /opt/homebrew)
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)
  endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
  if(NATIVE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Native targets default to Debug" FORCE)
  else()
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Cross-build default to Release" FORCE)
  endif()
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

#   ____        _ _     _            _   _   _                 
#  | __ ) _   _(_) | __| |  ___  ___| |_| |_(_)_ __   __ _ ___ 
#  |  _ \| | | | | |/ _` | / __|/ _ \ __| __| | '_ \ / _` / __|
#  | |_) | |_| | | | (_| | \__ \  __/ |_| |_| | | | | (_| \__ \
#  |____/ \__,_|_|_|\__,_| |___/\___|\__|\__|_|_| |_|\__, |___/
#                                                    |___/     
# COMPILE OPTIONS
add_compile_options(-fPIC -D_GNU_SOURCE -Wno-backslash-newline-escape)
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  message(STATUS "Debug mode, enabling all warnings")
  add_compile_options(-Wall -Wno-comment)
endif()
# Language Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
# RPATH management
# This embeds the relative path of the dynamic library into the executable
# So that it can find the library across installations
# If this is not given, the executable only searches the library in its same 
# folder or in a system path (/usr/lib, /usr/local/lib)
if (APPLE)
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
else()
  set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib;/usr/local/lib")
endif()
# FILES
# expect all sources in /src, except mains
set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
file(GLOB LIB_SOURCES_CPP "${SOURCE_DIR}/[^_]*.cpp")
file(GLOB LIB_SOURCES "${SOURCE_DIR}/[^_]*.c")
# generate defines.h
configure_file(
  ${SOURCE_DIR}/defines.h.in
  ${SOURCE_DIR}/defines.h
)

#   _____                    _       
#  |_   _|_ _ _ __ __ _  ___| |_ ___ 
#    | |/ _` | '__/ _` |/ _ \ __/ __|
#    | | (_| | | | (_| |  __/ |_\__ \
#    |_|\__,_|_|  \__, |\___|\__|___/
#                 |___/              
add_executable(ini_test ${SOURCE_DIR}/main/ini_test.c)
add_executable(mqtt_test ${SOURCE_DIR}/main/mqtt_test.c)
add_executable(mqtt_stress ${SOURCE_DIR}/main/mqtt_stress.c)
add_executable(c-cnc ${SOURCE_DIR}/main/c-cnc.c)

list(APPEND TARGETS_LIST
  ini_test
  mqtt_test
  mqtt_stress
  c-cnc
)

if(NATIVE) # Native build: use shared libraries
  add_library(${PROJECT_NAME}_shared SHARED ${LIB_SOURCES} ${LIB_SOURCES_CPP})
  list(APPEND TARGETS_LIST ${PROJECT_NAME}_shared)
  target_link_libraries(${PROJECT_NAME}_shared mosquitto)
  target_link_libraries(ini_test ${PROJECT_NAME}_shared)
  target_link_libraries(mqtt_test ${PROJECT_NAME}_shared mosquitto)
  target_link_libraries(mqtt_stress ${PROJECT_NAME}_shared mosquitto)
  target_link_libraries(c-cnc ${PROJECT_NAME}_shared m)
else() # X-build: use static libraries
  add_library(${PROJECT_NAME}_static STATIC ${LIB_SOURCES} ${LIB_SOURCES_CPP})
  target_link_libraries(ini_test ${PROJECT_NAME}_static)
  target_link_libraries(mqtt_test ${PROJECT_NAME}_static mosquitto_static ssl crypto dl pthread)
  target_link_libraries(mqtt_stress ${PROJECT_NAME}_static mosquitto_static ssl crypto dl pthread)
  target_link_libraries(c-cnc ${PROJECT_NAME}_static mosquitto_static ssl crypto dl pthread m)
endif()

# Copy cross compiled install products onto target system
if(NOT TARGET_HOSTNAME)
  SET(TARGET_HOSTNAME debian@beaglebone.local)
endif()
add_custom_target(scp 
  COMMAND  "scp" "-r" "products/*" "${TARGET_HOSTNAME}:"
  COMMENT "Copying products to ${TARGET_HOSTNAME}"
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)
add_dependencies(scp c-cnc)

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
if(NATIVE)
  set(TARGET_NAME CACHE STRING "Select TARGET platform")
  if(NOT TARGET_NAME)
    set(TARGET_NAME "armv7a")
  endif()
  set_property(CACHE TARGET_NAME PROPERTY STRINGS mipsel armv6 armv7 armv7-lts armv7a arm64 arm64-lts)
  set(TARGET_VERSION CACHE STRING "Select TARGET version")
  if(NOT TARGET_VERSION)
    if(TARGET_NAME MATCHES "armv7a")
      set(TARGET_VERSION "20210625-795dd4d")
    else()
      set(TARGET_VERSION "latest")
    endif()
  endif()
  set_property(CACHE TARGET_VERSION PROPERTY STRINGS latest 20210625-795dd4d)
  if(TARGET_NAME MATCHES "arm64")
    set(OPENSSL_LINUX_TYPE "linux-generic64")
  else()
    set(OPENSSL_LINUX_TYPE "linux-generic32")
  endif()
  configure_file(
    ${CMAKE_HOME_DIRECTORY}/Dockerfile.in
    ${CMAKE_HOME_DIRECTORY}/Dockerfile
    @ONLY
  )
  message(STATUS "Host Dockerfile generated for target ${TARGET_NAME}:${TARGET_VERSION}")
  message(NOTICE "   * Build with: docker build -t ${TARGET_NAME} .")
  message(NOTICE "   * then:       docker run --rm ${TARGET_NAME} > ${TARGET_NAME} && chmod a+x ${TARGET_NAME}")
endif()
