#pragma once


enum GameState {
  READY,
  LOADING_SESSION,
  FADING_OUT,
  FADING_IN
};

enum EntityType {
  ACTOR,
  COMBATANT,
  MAP_TRANSITION,
  PICKUP
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
  DELETE_ENTITY,
  CHANGE_SUPPLIES,
  ADD_SUPPLIES,
  CHANGE_PLR_LIFE,
};

enum CommandType {
  CHANGE_MAP,
  SAVE,
  LOAD,
  DELETE_ENT,
  SET_SUPPLIES,
  SET_LIFE,
};
