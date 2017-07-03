cmake_minimum_required(VERSION 2.8.9)

project(Decom CXX)

set(EXECUTABLE_OUTPUT_PATH ../bin/x64)
set(CMAKE_BUILD_TYPE release)

IF(WIN32)
  set(CMAKE_CXX_FLAGS "/c /Zi /GS- /GL /W3 /Gy /Gm /Ox /Oi /EHsc /W3 /GL /O2 /MD")
  set (CMAKE_EXE_LINKER_FLAGS "/LTCG /DEBUG /INCREMENTAL:NO")
ELSE()
  set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
ENDIF()
include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(Decom ${SOURCES})