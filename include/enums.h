#pragma once


enum class GameState {
  READY,
  LOADING_SESSION,
  INIT_COMBAT,
  END_COMBAT,
  FADING_OUT,
  FADING_IN
};

enum class SceneID {
  FIELD,
  COMBAT
};

enum EntityType {
  ACTOR,
  PICKUP,
  MAP_TRANSITION,
  COMBATANT,
};

enum class ActorType {
  PLAYER,
  COMPANION,
  ENEMY,
};

enum class SubWeaponID {
  KNIFE
};

enum class CompanionID {
  ERWIN
};

enum class CombatantType {
  PLAYER,
  COMPANION,
  ENEMY
};

enum CombatantState {
  NEUTRAL,
  HIT_STUN,
  MORALE_STUN,
  DEAD,
  WIND_UP,
  ACTION,
  END_LAG
};

enum Direction {
  UP = -2,
  LEFT = -1,
  RIGHT = 1,
  DOWN = 2,
};

enum class PickupType {
  SUPPLIES
};

enum class FieldEVT {
  LOAD_MAP,
  SAVE_SESSION,
  LOAD_SESSION,
  UPDATE_COMMON_DATA,
  DELETE_ENTITY,
  CHANGE_SUPPLIES,
  ADD_SUPPLIES,
  CHANGE_PLR_LIFE,
};

enum class ActorEVT {
  PLR_MOVING,
  PICKUP_IN,
  PICKUP_OUT,
};

enum class CommandType {
  CHANGE_MAP,
  SAVE,
  LOAD,
  DELETE_ENT,
  SET_SUPPLIES,
  SET_LIFE,
};
