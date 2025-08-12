#pragma once


enum class GameState {
  READY,
  LOADING_SESSION,
  INIT_COMBAT,
  END_COMBAT,
  FADING_OUT,
  FADING_IN,
  SLEEP,
};

enum class SceneID {
  TITLE,
  FIELD,
  COMBAT
};

enum class PanelID {
  CONFIG,
};

enum EntityType {
  ACTOR,
  PICKUP,
  MAP_TRANSITION,
  DMG_NUMBER,
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

enum class CombatantTeam {
  PARTY,
  ENEMY
};

enum class PartyMemberID {
  MARY,
  ERWIN,
};

enum class EnemyID {
  DUMMY,
};

enum CombatantState {
  ACTION,
  NEUTRAL,
  HIT_STUN,
  DEAD
};

enum class DamageType {
  LIFE,
  MORALE,
};

enum class StunType {
  NORMAL,
  DEFENSIVE,
  STAGGER
};

enum class ActionID {
  ATTACK,
  GHOST_STEP,
  EVADE
};

enum ActionPhase {
  WIND_UP,
  ACTIVE,
  END_LAG
};

enum class ActionType {
  OFFENSE_HP,
  OFFENSE_MP,
  DEFENSE,
  SUPPORT,
  SPECIAL
};

enum class StatusID {
  NONE = -1,
  CRIPPLED_ARM,
  CRIPPED_LEG,
  BROKEN,
  BLEEDING, 
  FEAR,
  DESPONDENT,
  MENDING,
  REFRESHED,
  ENDURANCE
};

enum class StatusType {
  POSITIVE,
  NEGATIVE
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
  INIT_COMBAT,
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

enum class CombatEVT {
  CREATE_DMG_NUM,
  DELETE_ENTITY,
};

enum class CommandType {
  CHANGE_MAP,
  SAVE,
  INIT_COMBAT,
  DELETE_ENT,
  SET_SUPPLIES,
  SET_LIFE,
};
