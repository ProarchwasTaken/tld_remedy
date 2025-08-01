cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

project("remedy" 
VERSION 1.6.0
HOMEPAGE_URL "https://github.com/ProarchwasTaken/tld_remedy"
LANGUAGES CXX)
option(BUILD_DEV "Build the game for development purposes." OFF)

if(${BUILD_DEV})
  message(STATUS "Using debug flags.")
  set(COMPILE_OPTIONS --debug -O0)
  set(PROJECT_STAGE "Pre-Alpha (Devmode)")
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
"include/base/combatant.h"
"include/base/combat_action.h"
"include/base/status_effect.h"
"include/base/party_member.h"
"include/base/enemy.h"
"include/data/keybinds.h"
"include/data/rect_ex.h"
"include/data/line.h"
"include/data/actor.h"
"include/data/field_event.h"
"include/data/actor_event.h"
"include/data/combat_event.h"
"include/data/session.h"
"include/data/animation.h"
"include/data/damage.h"
"include/utils/input.h"
"include/utils/collision.h"
"include/utils/text.h"
"include/utils/animation.h"
"include/system/sprite_atlas.h"
"include/system/sound_atlas.h"
"include/scenes/field.h"
"include/field/system/field_map.h"
"include/field/system/camera.h"
"include/field/system/field_handler.h"
"include/field/system/actor_handler.h"
"include/field/actors/player.h"
"include/field/actors/companion.h"
"include/field/actors/enemy.h"
"include/field/entities/map_trans.h"
"include/field/entities/pickup.h"
"include/scenes/combat.h"
"include/combat/system/stage.h"
"include/combat/system/camera.h"
"include/combat/system/evt_handler.h"
"include/combat/hud/life.h"
"include/combat/entities/dmg_number.h"
"include/combat/combatants/party/mary.h"
"include/combat/combatants/enemy/dummy.h"
"include/combat/actions/attack.h"
"include/combat/actions/ghost_step.h"
"include/combat/actions/evade.h"
)
set(SOURCE
"src/game.cpp"
"src/base/entity.cpp"
"src/base/actor.cpp"
"src/base/combatant.cpp"
"src/base/combat_action.cpp"
"src/base/status_effect.cpp"
"src/base/party_member.cpp"
"src/base/enemy.cpp"
"src/utils/input.cpp"
"src/utils/collision.cpp"
"src/utils/text.cpp"
"src/utils/animation.cpp"
"src/system/sprite_atlas.cpp"
"src/system/sound_atlas.cpp"
"src/scenes/field.cpp"
"src/field/system/field_map.cpp"
"src/field/system/camera.cpp"
"src/field/system/field_handler.cpp"
"src/field/system/actor_handler.cpp"
"src/field/actors/player.cpp"
"src/field/actors/companion.cpp"
"src/field/actors/enemy.cpp"
"src/field/entities/map_trans.cpp"
"src/field/entities/pickup.cpp"
"src/scenes/combat.cpp"
"src/combat/system/stage.cpp"
"src/combat/system/camera.cpp"
"src/combat/system/evt_handler.cpp"
"src/combat/hud/life.cpp"
"src/combat/entities/dmg_number.cpp"
"src/combat/combatants/party/mary.cpp"
"src/combat/combatants/enemy/dummy.cpp"
"src/combat/actions/attack.cpp"
"src/combat/actions/ghost_step.cpp"
"src/combat/actions/evade.cpp"
)

if(${BUILD_DEV})
  message(STATUS "Including debug source and header files.")
  list(APPEND HEADERS
  "include/field/system/field_commands.h"
  )
  list(APPEND SOURCE
  "src/field/system/field_commands.cpp"
  )
endif()


add_executable("remedy" src/main.cpp ${HEADERS} ${SOURCE})
target_link_libraries("remedy" PRIVATE ${RAYLIB})
target_compile_options("remedy" PRIVATE ${COMPILE_OPTIONS} ${META_DATA})

