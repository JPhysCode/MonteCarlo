cmake_minimum_required(VERSION 3.15)
project(MonteCarloTransport LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Core library with reusable components
add_library(core STATIC
    src/rng.cpp
    src/io.cpp
)

target_include_directories(core PUBLIC include)

# Quarter-circle Pi estimator executable
add_executable(quartercircle apps/quartercircle/main.cpp)

target_link_libraries(quartercircle PRIVATE core)

# Buffon's Needle executable (stub)
add_executable(buffonsneedle apps/buffonsneedle/main.cpp)

target_link_libraries(buffonsneedle PRIVATE core)
