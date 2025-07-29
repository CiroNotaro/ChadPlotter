cmake_minimum_required(VERSION 3.16)
project(ChadPlotter)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main
)
FetchContent_MakeAvailable(SDL3)

add_executable(ChadPlotter main.cpp)
target_include_directories(ChadPlotter PRIVATE include)
target_link_libraries(ChadPlotter PRIVATE SDL3::SDL3)
target_compile_features(ChadPlotter PRIVATE cxx_std_17)

add_library(ChadNet STATIC ChadNet.cpp include/ChadNet.h)
target_include_directories(ChadNet PRIVATE include)
target_compile_features(ChadNet PRIVATE cxx_std_17)

add_executable(Example01_Client examples/client.cpp)
target_include_directories(Example01_Client PRIVATE include)
target_link_libraries(Example01_Client PRIVATE ChadNet)
target_compile_features(Example01_Client PRIVATE cxx_std_17)

add_executable(Example02_Lines examples/client_lines.cpp)
target_include_directories(Example02_Lines PRIVATE include)
target_link_libraries(Example02_Lines PRIVATE ChadNet)
target_compile_features(Example02_Lines PRIVATE cxx_std_17)