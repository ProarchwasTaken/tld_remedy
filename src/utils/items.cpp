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
    case ItemID::FA_KIT: {
      return "First Aid Kit";
    }
    case ItemID::J_BOOK: {
      return "Torn Book";
    }
    case ItemID::J_BROOM: {
      return "Worn Broom";
    }
    case ItemID::J_SHIRT: {
      return "Gothic Shirt";
    }
    case ItemID::J_PANTS: {
      return "Gothic Pants";
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
    case ItemID::FA_KIT: {
      return "F.A.Kit";
    }
    case ItemID::J_BOOK: {
      return "T.Book";
    }
    case ItemID::J_BROOM: {
      return "W.Broom";
    }
    case ItemID::J_SHIRT: {
      return "G.Shirt";
    }
    case ItemID::J_PANTS: {
      return "G.Pants";
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
      "Restores a small percentage of\n"
      "a party member's Life.\n"
      "The amount healed is governed\n"
      "by the combatant's Recovery.";
    }
    case ItemID::M_SPLINT: {
      return
      "Can fix Broken Arm,\n"
      "Crippled Leg, and Mangled.\n"
      "Also cures the Despondent status\n"
      "ailment in combat.";
    }
    case ItemID::S_BANDAGE: {
      return
      "Restores a large percentage of\n"
      "a party member's Life.\n"
      "The amount healed is governed\n"
      "by the combatant's Recovery.";
    }
    case ItemID::S_WATER: {
      return 
      "Boosts a combatant's Dexterity,\n"
      "Speed, and Recovery by 20%.\n"
      "Restores Morale upon use.\n"
      "Effect lasts for 20 seconds.";
    }
    case ItemID::P_KILLERS: {
      return 
      "Temporarily boosts Resilience\n"
      "by 20%, and negates the effects\n"
      "of certain status ailments.\n"
      "Effect also grants Tenacity.";
    }
    case ItemID::FA_KIT: {
      return 
      "Restores 6 Life of each Party\n"
      "Member. Mary's recovery has no\n"
      "negative influence over how much\n"
      "Life this item recovers.";
    }
    case ItemID::J_BOOK: {
      return 
      "It's contents are illegible.\n"
      "Make sense when you consider\n"
      "the true nature of this place.\n"
      "Only useful for recycling.";
    }
    case ItemID::J_BROOM: {
      return 
      "A broom that had long since\n"
      "outlived it's usefulness.\n"
      "Not exactly an effective weapon,\n"
      "but it is useful for recycling.";
    }
    case ItemID::J_SHIRT: {
      return 
      "Looks like something straight\n"
      "from the middle ages.\n"
      "Only useful for recycling, and\n"
      "as a spare shirt, of course.";
    }
    case ItemID::J_PANTS: {
      return 
      "Looks rather appealing to wear,\n"
      "but now is definitely not the time.\n"
      "Maybe later.\n"
      "Only useful for recycling.";
    }
    default: {
      assert(item != ItemID::NONE);
      return "DESCRIPTION NOT\nAVAILIABLE";
    }
  }
}
