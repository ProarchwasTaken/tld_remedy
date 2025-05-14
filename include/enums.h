#pragma once


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
  LOAD_MAP
};
