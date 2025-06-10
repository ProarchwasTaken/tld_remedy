#pragma once


enum GameState {
  READY,
  LOADING_SESSION,
  FADING_OUT,
  FADING_IN
};

enum EntityType {
  ACTOR,
  PICKUP,
  MAP_TRANSITION,
  COMBATANT,
};

enum ActorType {
  PLAYER,
  COMPANION,
  ENEMY,
};

enum Direction {
  UP = -2,
  LEFT = -1,
  RIGHT = 1,
  DOWN = 2,
};

enum PickupType {
  SUPPLIES
};

enum FieldEventType {
  LOAD_MAP,
  SAVE_SESSION,
  LOAD_SESSION,
  UPDATE_COMMON_DATA,
  DELETE_ENTITY,
  CHANGE_SUPPLIES,
  ADD_SUPPLIES,
  CHANGE_PLR_LIFE,
};

enum ActorEventType {
  PLR_MOVING,
  PICKUP_IN_RANGE,
  PICKUP_OUT_RANGE,
};

enum CommandType {
  CHANGE_MAP,
  SAVE,
  LOAD,
  DELETE_ENT,
  SET_SUPPLIES,
  SET_LIFE,
};
