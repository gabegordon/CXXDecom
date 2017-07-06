cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_BUILD_TYPE release)
set(CMAKE_CXX_FLAGS "-std=c++11 -lpthread")
project(Decom)
include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(Decom ${SOURCES})
TARGET_LINK_LIBRARIES(Decom
pthread
)

