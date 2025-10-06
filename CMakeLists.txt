cmake_minimum_required(VERSION 3.15)
project(MonteCarloTransport LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Core library with reusable components
add_library(core STATIC
    src/random_sampling.cpp
    src/io.cpp
    src/statistics.cpp
    src/helpers.cpp
    src/physics.cpp
    src/capture.cpp
    src/fission.cpp
    src/scattering.cpp
)

target_include_directories(core PUBLIC include)

# Nuclear transport simulation applications
add_executable(readnplot apps/readnplot/main.cpp)
target_link_libraries(readnplot PRIVATE core)

add_executable(nslowing apps/nslowing/main.cpp)
target_link_libraries(nslowing PRIVATE core)

add_executable(nmultiplication apps/nmultiplication/main.cpp)
target_link_libraries(nmultiplication PRIVATE core)

add_executable(critenrich apps/critenrich/main.cpp)
target_link_libraries(critenrich PRIVATE core)

add_executable(timedep apps/timedep/main.cpp)
target_link_libraries(timedep PRIVATE core)

add_executable(fourfactor apps/fourfactor/main.cpp)
target_link_libraries(fourfactor PRIVATE core)
