cmake_minimum_required(VERSION 3.10) # Specify the minimum required CMake version

project(ReactionParser VERSION 1.0) # Define the project name and optionally its version

# Add an executable target, specifying its name and source files
add_executable(ReactionParser
    src/main.c
)

# Linking math.h library
target_link_libraries(ReactionParser PRIVATE m)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})