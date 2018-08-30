#
# This is a CMake makefile.  You can find the cmake utility and
# information about it at http://www.cmake.org
#

cmake_minimum_required(VERSION 2.8.4)

PROJECT(driverless-car-eyebot)

include(dlib/dlib/cmake)
#include()
find_package(OpenCV REQUIRED) #return an address
link_directories("/home/pi/eyebot/lib")
#link_directories("/home/pi/eyebot/lib/dynamic")


set(CMAKE_BUILD_TYPE Release)
set(DLIB_NO_GUI_SUPPORT OFF)

option(USE_AVX_INSTRUCTIONS  "Compile your program with AVX instructions" OFF)

IF(USE_AVX_INSTRUCTIONS)
  add_definitions(-mavx)
  add_definitions(-march=native)
ENDIF()

MACRO(add_source name)
    ADD_EXECUTABLE(${name} src/${name}.cpp)
    TARGET_LINK_LIBRARIES(${name} dlib )
    target_link_libraries(${name} ${OpenCV_LIBS}) 
    target_link_libraries(${name} libeyebot.a)

ENDMACRO()

add_source(ftsdv07)
add_source(hog_detector)
add_source(detect)
add_source(eyebot)
