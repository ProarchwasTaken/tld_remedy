cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

project("remedy" LANGUAGES CXX)
option(BUILD_DEV "Build the game for development purposes." OFF)

if(${BUILD_DEV})
  message(STATUS "Using debug flags.")
  set(COMPILE_OPTIONS --debug -O0)
else()
  set(COMPILE_OPTIONS -DNDEBUG)
endif()

include_directories("include")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(RAYLIB -lraylib -lopengl32 -lgdi32 -lwinmm)
set(HEADERS 
"include/enums.h"
"include/game.h"
"include/base/scene.h"
"include/base/entity.h"
"include/data/rect_ex.h"
"include/scenes/debug_field.h"
)
set(SOURCE
"src/game.cpp"
"src/base/entity.cpp"
"src/scenes/debug_field.cpp"
)

add_executable("remedy" src/main.cpp ${HEADERS} ${SOURCE})
target_link_libraries("remedy" PRIVATE ${RAYLIB})
target_compile_options("remedy" PRIVATE ${COMPILE_OPTIONS})

