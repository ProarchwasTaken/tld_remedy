cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

project("remedy")

include_directories("include")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(RAYLIB -lraylib -lopengl32 -lgdi32 -lwinmm)
set(HEADERS 
"include/enums.h"
)

add_executable("remedy" src/main.cpp)
target_link_libraries("remedy" ${RAYLIB})

