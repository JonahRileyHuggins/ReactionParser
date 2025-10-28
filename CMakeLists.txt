cmake_minimum_required(VERSION 3.10) # Specify the minimum required CMake version

project(ReactionParser VERSION 1.0) # Define the project name and optionally its version

set(CMAKE_BUILD_TYPE "Debug")
add_compile_options(-O3 -march=znver4 -mavx2 -mavx -mavx512f -mavx512vl -mavx512cd -mavx512vl -mavx512bw -mavx512dq -mavx512ifma -mavx512vbmi -fopt-info-vec-optimized -fopt-info-vec-missed -fopt-info-vec-all)
add_executable(ReactionParser
    src/main.c
    src/parser.c
)

add_executable(test_reactionparser
    tests/test_reactionparser.c
    src/parser.c
)

add_executable(speed
    tests/speedtest.c
    src/parser.c
)

# Linking math.h library
if(UNIX)
    target_link_libraries(ReactionParser m)
    target_link_libraries(test_reactionparser m)
    target_link_libraries(speed m)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})