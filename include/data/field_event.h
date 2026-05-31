#pragma once
#include <string>
#include <vector>
#include "enums.h"
#include "scenes/camp_menu.h"

struct FieldEvent {
  FieldEVT event_type;
};

struct LoadMapEvent : FieldEvent {
  std::string map_name;
  std::string spawn_point;
};

struct OpenMenuSCEvent : FieldEvent {
  CampMenuOption shortcut;
};

struct InitCombatEvent : FieldEvent {
  TroopID id = TroopID::INVALID;
};

struct InitCombatFEvent : FieldEvent {
  TroopID id;
  int reward = 0;
};

struct GameOverEvent : FieldEvent {
  std::string reason;
};

struct UpdateCommonEvent : FieldEvent {
  int object_id;
  bool active;
};

struct MarkAsDeadEvent : FieldEvent {
  int object_id;
};

struct DeleteEntityEvent : FieldEvent {
  int entity_id;
};

struct SetSuppliesEvent : FieldEvent {
  int value;
};

struct AddSuppliesEvent : FieldEvent {
  int magnitude;
};

struct SetLifeEvent : FieldEvent {
  float value;
};

struct AddItemEvent : FieldEvent {
  ItemID item;
};

struct RemoveItemEvent : FieldEvent {
  ItemID item;
};

struct AddEffectEvent : FieldEvent {
  StatusID effect_id;
};

struct RemoveEffectEvent : FieldEvent {
  StatusID effect_id;
};

struct OpenDialogEvent : FieldEvent {
  std::vector<std::string> dialog;
  bool end_prompt = false;
};

struct OpenDialogEventEx : FieldEvent {
  std::string name;
  std::string title;
  std::vector<std::string> dialog;
  bool end_prompt = false;
};

struct StartSequenceEvent : FieldEvent {
  SequenceID sequence;
};

struct StartFSequenceEvent : FieldEvent {
  SequenceID sequence;
  FlagID flag = FlagID::NONE;
};

