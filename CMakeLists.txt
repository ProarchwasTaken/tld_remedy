cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

project("remedy" 
VERSION 0.1.0
HOMEPAGE_URL "https://github.com/ProarchwasTaken/tld_remedy"
LANGUAGES CXX)
option(BUILD_DEV "Build the game for development purposes." OFF)

if(${BUILD_DEV})
  message(STATUS "Using debug flags.")
  set(COMPILE_OPTIONS --debug -O0)
  set(PROJECT_STAGE "Alpha (Devmode)")
else()
  set(COMPILE_OPTIONS -DNDEBUG)
  set(PROJECT_STAGE "Alpha")
endif()

if(WIN32)
  set(PLATFORM 0)
else()
  set(PLATFORM 1)
endif()

set(META_DATA
-DVERSION="${CMAKE_PROJECT_VERSION}"
-DVER_STAGE="${PROJECT_STAGE}"
-DPLATFORM=${PLATFORM}
-DAUTHOR="Tyler Dillard"
-DHOME_URL="${CMAKE_PROJECT_HOMEPAGE_URL}"
)

include_directories("include")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(HEADERS 
"include/enums.h"
"include/game.h"
"include/base/scene.h"
"include/base/panel.h"
"include/base/entity.h"
"include/base/actor.h"
"include/base/field_sequence.h"
"include/base/combatant.h"
"include/base/combat_action.h"
"include/base/status_effect.h"
"include/base/party_member.h"
"include/base/enemy.h"
"include/base/sub_weapon.h"
"include/data/keybinds.h"
"include/data/rect_ex.h"
"include/data/line.h"
"include/data/actor.h"
"include/data/field_event.h"
"include/data/actor_event.h"
"include/data/combat_event.h"
"include/data/combatant_event.h"
"include/data/session.h"
"include/data/personal.h"
"include/data/animation.h"
"include/data/damage.h"
"include/data/enemy_troops.h"
"include/data/technique.h"
"include/data/ai_behavior.h"
"include/utils/input.h"
"include/utils/collision.h"
"include/utils/text.h"
"include/utils/animation.h"
"include/utils/menu.h"
"include/utils/comparisons.h"
"include/utils/math.h"
"include/system/sprite_atlas.h"
"include/system/sound_atlas.h"
"include/system/noise_effect.h"
"include/system/music_player.h"
"include/scenes/title.h"
"include/scenes/camp_menu.h"
"include/scenes/rest_menu.h"
"include/scenes/game_over.h"
"include/menu/panels/config.h"
"include/menu/panels/confirm.h"
"include/menu/panels/remap.h"
"include/menu/panels/status.h"
"include/menu/panels/items.h"
"include/menu/panels/tech.h"
"include/menu/panels/dialog.h"
"include/menu/panels/diagnose.h"
"include/menu/panels/crafting.h"
"include/menu/hud/party.h"
"include/menu/hud/portrait.h"
"include/menu/hud/reticle.h"
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
"include/field/entities/save_point.h"
"include/field/sequences/save.h"
"include/field/sequences/rest.h"
"include/scenes/combat.h"
"include/combat/system/stage.h"
"include/combat/system/camera.h"
"include/combat/system/evt_handler.h"
"include/combat/system/cbt_handler.h"
"include/combat/hud/life.h"
"include/combat/hud/enemy.h"
"include/combat/hud/combo.h"
"include/combat/hud/toasts.h"
"include/combat/hud/blackbars.h"
"include/combat/hud/cmd_plr.h"
"include/combat/hud/cmd_assist.h"
"include/combat/hud/cmd_item.h"
"include/combat/entities/dmg_number.h"
"include/combat/entities/status_text.h"
"include/combat/entities/afterimage.h"
"include/combat/sub_weapons/knife.h"
"include/combat/combatants/party/mary.h"
"include/combat/combatants/party/erwin.h"
"include/combat/combatants/enemy/dummy.h"
"include/combat/combatants/enemy/servant.h"
"include/combat/actions/attack.h"
"include/combat/actions/ghost_step.h"
"include/combat/actions/evade.h"
"include/combat/actions/knife_cleave.h"
"include/combat/actions/knife_piercer.h"
"include/combat/actions/use_item.h"
"include/combat/actions/provoke.h"
"include/combat/actions/3rd_party.h"
"include/combat/status_effects/broken_arm.h"
"include/combat/status_effects/crippled_leg.h"
"include/combat/status_effects/mangled.h"
"include/combat/status_effects/despondent.h"
"include/combat/status_effects/mending.h"
"include/combat/status_effects/refreshed.h"
"include/combat/status_effects/endurance.h"
)

set(SOURCE
"src/main.cpp"
"src/game.cpp"
"src/base/panel.cpp"
"src/base/entity.cpp"
"src/base/actor.cpp"
"src/base/field_sequence.cpp"
"src/base/combatant.cpp"
"src/base/combat_action.cpp"
"src/base/status_effect.cpp"
"src/base/party_member.cpp"
"src/base/enemy.cpp"
"src/base/sub_weapon.cpp"
"src/utils/input.cpp"
"src/utils/collision.cpp"
"src/utils/text.cpp"
"src/utils/animation.cpp"
"src/utils/comparisons.cpp"
"src/utils/math.cpp"
"src/system/sprite_atlas.cpp"
"src/system/sound_atlas.cpp"
"src/system/noise_effect.cpp"
"src/system/music_player.cpp"
"src/scenes/title.cpp"
"src/scenes/camp_menu.cpp"
"src/scenes/rest_menu.cpp"
"src/scenes/game_over.cpp"
"src/menu/panels/config.cpp"
"src/menu/panels/confirm.cpp"
"src/menu/panels/remap.cpp"
"src/menu/panels/status.cpp"
"src/menu/panels/items.cpp"
"src/menu/panels/tech.cpp"
"src/menu/panels/dialog.cpp"
"src/menu/panels/diagnose.cpp"
"src/menu/panels/crafting.cpp"
"src/menu/hud/party.cpp"
"src/menu/hud/portrait.cpp"
"src/menu/hud/reticle.cpp"
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
"src/field/entities/save_point.cpp"
"src/field/sequences/save.cpp"
"src/field/sequences/rest.cpp"
"src/scenes/combat.cpp"
"src/combat/system/stage.cpp"
"src/combat/system/camera.cpp"
"src/combat/system/evt_handler.cpp"
"src/combat/system/cbt_handler.cpp"
"src/combat/hud/life.cpp"
"src/combat/hud/enemy.cpp"
"src/combat/hud/combo.cpp"
"src/combat/hud/toasts.cpp"
"src/combat/hud/blackbars.cpp"
"src/combat/hud/cmd_plr.cpp"
"src/combat/hud/cmd_assist.cpp"
"src/combat/hud/cmd_item.cpp"
"src/combat/entities/dmg_number.cpp"
"src/combat/entities/status_text.cpp"
"src/combat/entities/afterimage.cpp"
"src/combat/sub_weapons/knife.cpp"
"src/combat/combatants/party/mary.cpp"
"src/combat/combatants/party/erwin.cpp"
"src/combat/combatants/enemy/dummy.cpp"
"src/combat/combatants/enemy/servant.cpp"
"src/combat/actions/attack.cpp"
"src/combat/actions/ghost_step.cpp"
"src/combat/actions/evade.cpp"
"src/combat/actions/knife_cleave.cpp"
"src/combat/actions/knife_piercer.cpp"
"src/combat/actions/use_item.cpp"
"src/combat/actions/provoke.cpp"
"src/combat/actions/3rd_party.cpp"
"src/combat/status_effects/broken_arm.cpp"
"src/combat/status_effects/crippled_leg.cpp"
"src/combat/status_effects/mangled.cpp"
"src/combat/status_effects/despondent.cpp"
"src/combat/status_effects/mending.cpp"
"src/combat/status_effects/refreshed.cpp"
"src/combat/status_effects/endurance.cpp"
)

if(${BUILD_DEV})
  message(STATUS "Including debug source and header files.")
  list(APPEND HEADERS
  "include/field/system/field_commands.h"
  "include/field/sequences/db_01.h"
  )
  list(APPEND SOURCE
  "src/field/system/field_commands.cpp"
  "src/field/sequences/db_01.cpp"
  )
endif()


add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCE})
if(WIN32)
  target_link_libraries(${PROJECT_NAME} -lraylib -lopengl32 -lgdi32 
    -lwinmm)
elseif(APPLE)
  find_package(raylib 5.0 QUIET)
  target_link_libraries(${PROJECT_NAME} "-framework IOKit")
  target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
  target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
  target_link_libraries(${PROJECT_NAME} raylib)
else()
  find_package(raylib 5.0 QUIET)
  target_link_libraries(${PROJECT_NAME} raylib)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS} 
  ${META_DATA})

