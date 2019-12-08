cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_BUILD_TYPE "Release")
#set(CMAKE_BUILD_TYPE "Debug")

project(MTViewer)

# OPTIONS
set(CMAKE_BUILD_TYPE Release)
add_compile_options(-fPIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++14>)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  message(STATUS "Debug mode, enabling all warnings. Platform: ${CMAKE_HOST_SYSTEM_NAME}")
  add_definitions(-Wall -Wno-comment -Wno-deprecated-declarations)
endif()

file(GLOB APP_SOURCES ${CMAKE_HOME_DIRECTORY}/viewer/*.c*)
set(ATB_SOURCES 
  AntTweakBar/TwColors.cpp
  AntTweakBar/TwFonts.cpp
  AntTweakBar/TwOpenGL.cpp
  AntTweakBar/TwOpenGLCore.cpp
  AntTweakBar/TwBar.cpp
  AntTweakBar/TwMgr.cpp
  AntTweakBar/TwPrecomp.cpp
  AntTweakBar/LoadOGL.cpp
  AntTweakBar/LoadOGLCore.cpp
  AntTweakBar/TwEventGLFW.c
  AntTweakBar/TwEventGLUT.c
  AntTweakBar/TwEventSDL.c
  AntTweakBar/TwEventSDL12.c
  AntTweakBar/TwEventSDL13.c
  AntTweakBar/TwEventSFML.cpp
)
if(UNIX AND NOT APPLE)
  list(APPEND ATB_SOURCES AntTweakBar/TwEventX11.c)
endif()
message(STATUS Sources: ${APP_SOURCES} ${ATB_SOURCES})

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
add_executable(MTViewer ${APP_SOURCES} ${ATB_SOURCES})
target_include_directories(MTViewer PRIVATE
  ${OPENGL_INCLUDE_DIR}
  ${GLUT_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Viewer
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(UNIX AND NOT APPLE)
  target_compile_options(MTViewer PUBLIC -Wno-tautological-compare)
  target_compile_definitions(MTViewer PUBLIC -D_GNU_SOURCE -D_UNIX)
  target_link_libraries(MTViewer
    X11
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
  )
elseif(APPLE)
  target_compile_options(MTViewer PUBLIC -std=gnu++11 -ObjC++ -Wno-deprecated-declarations)
  target_compile_definitions(MTViewer PUBLIC
    -D_MACOSX -D__PLACEMENT_NEW_INLINE 
    -DGL_SILENCE_DEPRECATION
  )
  target_link_libraries(MTViewer
    ${X11_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
  )
endif()

# INSTALL
set(CMAKE_INSTALL_PREFIX ${CMAKE_HOME_DIRECTORY}/..)
install(TARGETS MTViewer
  RUNTIME DESTINATION bin
)
