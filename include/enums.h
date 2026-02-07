#pragma once


enum class SceneID {
  TITLE,
  FIELD,
  CAMP_MENU,
  REST_MENU,
  COMBAT,
  GAME_OVER
};

enum class PanelID {
  CONFIG,
  CONFIRM,
  REMAP,
  STATUS,
  ITEMS,
  TECH,
  DIALOG
};

enum EntityType {
  ACTOR,
  PICKUP,
  SAVE_POINT,
  MAP_TRANSITION,
  DMG_NUMBER,
  STATUS_TEXT,
  COMBATANT,
  AFTERIMAGE
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

enum class ItemID {
  NONE = -1,
  I_BANDAGE,
  M_SPLINT,
  S_BANDAGE,
  S_WATER,
  P_KILLERS,
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
  DUMMY = -1,
  SERVANT
};

enum class TroopID {
  INVALID = -1,
  DB_TROOP1,
  DB_TROOP2,
  DB_TROOP3,
  DB_TROOP4,
  DB_TROOP5
};

enum class ActionID {
  ATTACK,
  GHOST_STEP,
  EVADE,
  KNIFE_CLEAVE,
  KNIFE_PIERCER,
  USE_ITEM,
  ERWIN_PROVOKE,
  ERWIN_3RD_PARTY
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
  INIT_COMBAT_FORCED,
  GOTO_TITLE,
  GAME_OVER,
  UPDATE_COMMON_DATA,
  DELETE_ENTITY,
  CHANGE_SUPPLIES,
  ADD_SUPPLIES,
  CHANGE_PLR_LIFE,
  CHANGE_COM_LIFE,
  ADD_ITEM,
  REMOVE_ITEM,
  CLEAR_INV,
  PLR_ADD_EFFECT,
  PLR_RM_EFFECT,
  COM_ADD_EFFECT,
  COM_RM_EFFECT,
  OPEN_DIALOG,
  START_SEQUENCE
};

enum class ActorEVT {
  PLR_MOVING,
  INTERACT_IN,
  INTERACT_OUT,
};

enum class SequenceID {
  #ifndef NDEBUG
  DB_01, 
  #endif // !NDEBUG

  SAVE
};

enum class CombatEVT {
  CREATE_DMG_NUM,
  CREATE_STAT_TXT,
  CREATE_AFTERIMAGE,
  DELETE_ENTITY,
  OPEN_ITEM_HUD,
  REMOVE_ITEM,
  START_TOAST,
  BAR_SET,
  BAR_SET_TARGET,
  BAR_RESET
};

enum class CombatantEVT {
  TOOK_DAMAGE,
  MORALE_GAINED,
  EFFECT_GAINED,
  EFFECT_LOST,
  WARNING
};

