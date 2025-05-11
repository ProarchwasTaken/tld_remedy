#pragma once


enum EntityType {
  ACTOR,
  COMBATANT,
};

enum ActorType {
  PLAYER,
  COMPANION,
  ENEMY,
};

enum SpawnType {
  INITIAL,
  TRANSITION
};

enum Direction {
  UP = -2,
  LEFT = -1,
  RIGHT = 1,
  DOWN = 2,
};
