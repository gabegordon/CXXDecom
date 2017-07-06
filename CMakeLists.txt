cmake_minimum_required(VERSION 2.8.9)

project(Decom)

if (UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++11 -lpthread -Wall")
else ()
    set(CMAKE_CXX_FLAGS_RELEASE "/c /Zi /GS- /GL /W3 /Gy /Gm- /Ox /EHsc /W3 /O2 /GL /MD /favor:INTEL64")
    set(CMAKE_EXE_LINKER_FLAGS "/LTCG /DEBUG")
endif ()

set(EXECUTABLE_OUTPUT_PATH ../bin/x64)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(Decom ${SOURCES})
if (UNIX)
TARGET_LINK_LIBRARIES(Decom
pthread
)
endif ()

