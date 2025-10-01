#pragma once


enum class SceneID {
  TITLE,
  FIELD,
  CAMP_MENU,
  COMBAT
};

enum class PanelID {
  CONFIG,
  CONFIRM,
  STATUS
};

enum EntityType {
  ACTOR,
  PICKUP,
  MAP_TRANSITION,
  DMG_NUMBER,
  STATUS_TEXT,
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

enum class TechCostType {
  MORALE,
  LIFE
};

enum class EnemyID {
  DUMMY,
};

enum class ActionID {
  ATTACK,
  GHOST_STEP,
  EVADE,
  KNIFE_CLEAVE,
  KNIFE_PIERCER
};

enum class StatusID {
  NONE = -1,
  BROKEN_ARM,
  CRIPPLED_LEG,
  MANGLED,
  BLEEDING, 
  FEAR,
  DESPONDENT,
  MENDING,
  REFRESHED,
  ENDURANCE
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
  OPEN_MENU,
  INIT_COMBAT,
  GOTO_TITLE,
  UPDATE_COMMON_DATA,
  DELETE_ENTITY,
  CHANGE_SUPPLIES,
  ADD_SUPPLIES,
  CHANGE_PLR_LIFE,
  CHANGE_COM_LIFE
};

enum class ActorEVT {
  PLR_MOVING,
  PICKUP_IN,
  PICKUP_OUT,
};

enum class CombatEVT {
  CREATE_DMG_NUM,
  CREATE_STAT_TXT,
  DELETE_ENTITY,
};

enum class CombatantEVT {
  TOOK_DAMAGE,
};

enum class CommandType {
  CHANGE_MAP,
  SAVE,
  INIT_COMBAT,
  TITLE,
  DELETE_ENT,
  SET_SUPPLIES,
  SET_LIFE,
};
