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
  MAP_TRANSITION
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

enum FieldEventType {
  LOAD_MAP,
  CHANGE_SUPPLIES,
  CHANGE_PLR_LIFE,
  SAVE_SESSION,
  LOAD_SESSION,
};

enum CommandType {
  CHANGE_MAP,
  SET_SUPPLIES,
  SET_LIFE,
  SAVE,
  LOAD
};
