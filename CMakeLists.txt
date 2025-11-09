cmake_minimum_required(VERSION 3.10) # Specify the minimum required CMake version

project(ReactionParser VERSION 1.0) # Define the project name and optionally its version

set(CMAKE_BUILD_TYPE Release)
add_compile_options(
    -Ofast 
    -march=native 
    -mavx2 -mavx 
    -fopt-info-vec-optimized 
    -fopt-info-vec-missed 
    -fopt-info-vec-all
    )
    
add_executable(ReactionParser
    src/main.c
    src/parser.c
)
target_include_directories(ReactionParser PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(test_reactionparser
    tests/test_reactionparser.c
    src/parser.c
)
target_include_directories(test_reactionparser PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(speed
    tests/speedtest.c
    src/parser.c
)
target_include_directories(speed PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)

# Linking math.h library
if(UNIX)
    target_link_libraries(ReactionParser m)
    target_link_libraries(test_reactionparser m)
    target_link_libraries(speed m)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})