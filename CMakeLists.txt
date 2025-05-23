cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

project("remedy" 
VERSION 1.0.0
HOMEPAGE_URL "https://github.com/ProarchwasTaken/tld_remedy"
LANGUAGES CXX)
option(BUILD_DEV "Build the game for development purposes." OFF)

if(${BUILD_DEV})
  message(STATUS "Using debug flags.")
  set(COMPILE_OPTIONS --debug -O0)
  set(PROJECT_STAGE "Pre-Alpha | Devmode")
else()
  set(COMPILE_OPTIONS -DNDEBUG)
  set(PROJECT_STAGE "Pre-Alpha")
endif()

set(META_DATA
-DVERSION="${CMAKE_PROJECT_VERSION}"
-DVER_STAGE="${PROJECT_STAGE}"
-DAUTHOR="Tyler Dillard"
-DHOME_URL="${CMAKE_PROJECT_HOMEPAGE_URL}"
)

include_directories("include")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(RAYLIB -lraylib -lopengl32 -lgdi32 -lwinmm)
set(HEADERS 
"include/enums.h"
"include/game.h"
"include/base/scene.h"
"include/base/entity.h"
"include/base/actor.h"
"include/data/keybinds.h"
"include/data/rect_ex.h"
"include/data/line.h"
"include/data/actor.h"
"include/data/field_event.h"
"include/utils/input.h"
"include/utils/camera.h"
"include/utils/collision.h"
"include/system/field_handler.h"
"include/system/field_map.h"
"include/scenes/field.h"
"include/entities/map_trans.h"
"include/actors/player.h"
)
set(SOURCE
"src/game.cpp"
"src/base/entity.cpp"
"src/base/actor.cpp"
"src/utils/input.cpp"
"src/utils/camera.cpp"
"src/utils/collision.cpp"
"src/system/field_handler.cpp"
"src/system/field_map.cpp"
"src/scenes/field.cpp"
"src/entities/map_trans.cpp"
"src/actors/player.cpp"
)

if(${BUILD_DEV})
  message(STATUS "Including debug source and header files.")
  list(APPEND HEADERS
  "include/data/debug.h"
  "include/system/field_commands.h"
  )
  list(APPEND SOURCE
  "src/system/field_commands.cpp"
  )
endif()


add_executable("remedy" src/main.cpp ${HEADERS} ${SOURCE})
target_link_libraries("remedy" PRIVATE ${RAYLIB})
target_compile_options("remedy" PRIVATE ${COMPILE_OPTIONS} ${META_DATA})

