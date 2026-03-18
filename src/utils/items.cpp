#include <cassert>
#include <string>
#include "enums.h"
#include "utils/items.h"

using std::string;


string ItemUtils::getName(ItemID item) {
  switch (item) {
    case ItemID::I_BANDAGE: {
      return "Improvised Bandage";
    }
    case ItemID::M_SPLINT: {
      return "Makeshift Splint";
    }
    case ItemID::S_BANDAGE: {
      return "Sterilized Bandage";
    }
    case ItemID::S_WATER: {
      return "Sparkling Water";
    }
    case ItemID::P_KILLERS: {
      return "Painkillers";
    }
    default: {
      return "";
    }
  }
}

string ItemUtils::getShortened(ItemID item) {
  switch (item) {
    case ItemID::NONE: {
      return "--------";
    }
    case ItemID::I_BANDAGE: {
      return "I.Bandage";
    }
    case ItemID::M_SPLINT: {
      return "M.Splint";
    }
    case ItemID::S_BANDAGE: {
      return "S.Bandage";
    }
    case ItemID::S_WATER: {
      return "S.Water";
    }
    case ItemID::P_KILLERS: {
      return "P.Killers";
    }
    default: {
      return "N / A";
    }
  }
}

string ItemUtils::getDescription(ItemID item) {
  switch (item) {
    case ItemID::I_BANDAGE: {
      return 
      "Restores 35% of a combatant's\n"
      "Life.\n"
      "In Combat: Instead applies the\n"
      "Mending status effect.";
    }
    case ItemID::M_SPLINT: {
      return
      "Can fix Broken Arm,\n"
      "Crippled Leg, and Mangled.\n"
      "In Combat: Also cures the\n"
      "Despondent status ailment.";
    }
    case ItemID::S_BANDAGE: {
      return
      "Restores 50% of a combatant's\n"
      "Life.\n"
      "In Combat: Applies Mending\n"
      "that heals at a faster rate.";
    }
    case ItemID::S_WATER: {
      return 
      "Boosts a combatant's Dexterity,\n"
      "Speed, and Recovery by 20%.\n"
      "Effect lasts for 20 seconds.";
    }
    case ItemID::P_KILLERS: {
      return 
      "Temporarily boosts Resilience\n"
      "by 20%, and negates the effects\n"
      "of certain status ailments.\n"
      "Effect also grants Tenacity.";
    }
    default: {
      assert(item != ItemID::NONE);
      return "DESCRIPTION NOT\nAVAILIABLE";
    }
  }
}
