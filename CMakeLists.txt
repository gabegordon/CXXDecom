cmake_minimum_required(VERSION 2.8.9)

project(Decom CXX)

set(CMAKE_BUILD_TYPE release)
set(EXECUTABLE_OUTPUT_PATH ../bin/x64)

IF(WIN32)
  set(CMAKE_CXX_FLAGS "/c /Zi /GS- /GL /W3 /Gy /Gm- /O2 /EHsc /W3 /O2 /GL /MD /favor:INTEL64")
  set (CMAKE_EXE_LINKER_FLAGS "/LTCG /DEBUG")
ELSE()
  set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
ENDIF()
include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(Decom ${SOURCES})