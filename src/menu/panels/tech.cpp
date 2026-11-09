#include <cassert>
#include <cstddef>
#include <string>
#include <memory>
#include <array>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/panel.h"
#include "data/session.h"
#include "utils/input.h"
#include "utils/menu.h"
#include "utils/text.h"
#include "scenes/camp_menu.h"
#include "menu/panels/text_box.h"
#include "menu/panels/tech.h"
#include <plog/Log.h>

using std::string, std::array, std::make_unique;


TechsPanel::TechsPanel(Session *session, string *description, 
                       Color *desc_color) 
{
  id = PanelID::TECH;

  assert(description != NULL);
  this->description = description;
  *description = "Select a Party Member";

  assert(desc_color != NULL);
  this->desc_color = desc_color;

  options[0] = &session->player;
  options[1] = &session->companion;

  selected = options.begin();
  setupTech();

  menu_atlas = &Game::menu_atlas;
  menu_atlas->use();

  camp_atlas = &CampMenuScene::atlas;
  camp_atlas->use();

  med_font = &Game::med_font;
  med_size = med_font->baseSize;

  sfx = &Game::menu_sfx;
  sfx->use();

  keybinds = &Game::settings.menu_keybinds;
  PLOGI << "Techs Panel: Initialized";
}

TechsPanel::~TechsPanel() {
  menu_atlas->release();
  camp_atlas->release();
  sfx->release();
}

void TechsPanel::setupTech() {
  assert(selected != NULL);

  for (TechInfo &tech : tech_info) {
    tech.name.clear();
    tech.type.clear();
    tech.desc.clear();
    tech.category = TechCategory::NONE;
  }

  Character *party_member = *selected;
  array<TechInfo, 6>::iterator tech = tech_info.begin();
  tech_disallowed.clear();

  switch (party_member->member_id) {
    case PartyMemberID::MARY: {
      Player *mary = static_cast<Player*>(party_member);
      setupWeaponTech(mary->weapon_id, tech);
      tech++;

      tech->name = "Ghoststep";
      tech->type= "Defensive Action";
      tech->category = TechCategory::ACTION;
      tech->desc = 
        "HP COST: Usually 5.5\n"
        "INPUT: Press 'DEFENSIVE' while holding\n"
        "'LEFT' or 'RIGHT' on the D-PAD.\n\n"

        "Mary will dash forwards or backwards\n"
        "depending on the input used. While dashing,\n"
        "Mary is rendered intangible to any and all\n"
        "attacks.";

      tech++;

      tech->name = "Evade";
      tech->type = "Defensive Action";
      tech->category = TechCategory::ACTION;
      tech->desc = 
        "HP COST: Dependent on Mary's Dexterity\n"
        "INPUT: Press 'DEFENSIVE while holding\n"
        "'DOWN' on the D-PAD.\n\n"

        "After a brief windup, Mary braces himself\n"
        "to dodge the first attack that comes his way.\n"
        "If successful, any damage Mary would've\n"
        "taken will be converted to Exhaustion.";
      tech++;

      tech->name = "Humanity";
      tech->type = "Passive Trait";
      tech->category = TechCategory::PASSIVE;
      tech->desc = 
        "Mary's Attack can only inflict Morale damage.\n"
        "However, he gains Morale that is proportional\n"
        "to the damage dealt.\n\n"

        "If Mary isn't Despondent, a percentage of\n"
        "the Morale gained will be shared with all\n"
        "other party members.";
      tech++;

      tech->name = "Auto Evade";
      tech->type = "Passive Ability";
      tech->category = TechCategory::PASSIVE;
      tech->desc = 
        "Mary will automatically dodge attacks at the\n"
        "cost of Morale. The amount lost is dependent\n"
        "on his Persistence vs the Assailant's\n"
        "Intimidation.\n\n"

        "If Mary's Morale goes below 0, he will become\n"
        "Despondent. During which, Auto-Evade is\n"
        "easier to bypass.";
      break;
    }
    case PartyMemberID::ERWIN: {
      tech->name = "Provoke";
      tech->type = "Light Assist";
      tech->category = TechCategory::ASSIST;
      tech->desc = 
        "HP COST: 5 | COOLDOWN: 5s\n\n"

        "Erwin performs a taunt that draws the aggro\n"
        "of every enemy that happens to be targeting\n"
        "Mary. He also gains intangibility to any and\n"
        "all attacks for the duration of this action.";
      tech++;

      tech->name = "3rd Party";
      tech->type = "Heavy Assist";
      tech->category = TechCategory::ASSIST;
      tech->desc = 
        "MP COST: 10 | COOLDOWN: 8s\n"
        "SCALING: Offense\n\n"

        "Erwin runs to the enemy that Mary was\n"
        "targeting when the assist was called. Once\n"
        "they're within range, Erwin will try to\n"
        "dropkick said enemy.\n\n"

        "If the attack hits, the target will be knocked\n"
        "back a far distance, and become Vulnerable\n"
        "for a second or so.";
      tech++;

      tech->name = "Humanity";
      tech->type = "Passive Trait";
      tech->category = TechCategory::PASSIVE;
      tech->desc = 
        "Erwin's Attack can only inflict Morale damage.\n"
        "However, he gains Morale that is proportional\n"
        "to the damage dealt.\n\n"

        "If Erwin isn't Despondent, a percentage of\n"
        "the Morale gained will be shared with all\n"
        "other party members.";
      tech++;

      tech->name = "Auto Evade";
      tech->type = "Passive Ability";
      tech->category = TechCategory::PASSIVE;
      tech->desc = 
        "Erwin will automatically dodge attacks at the\n"
        "cost of Morale. The amount lost is dependent\n"
        "on his Persistence vs the Assailant's\n"
        "Intimidation.\n\n"

        "If Erwin's Morale goes below 0, he will become\n"
        "Despondent. During which, Auto-Evade is\n"
        "easier to bypass.";
      break;
    }
    case PartyMemberID::XANDER: {
      tech->name = "Tail Whip";
      tech->type = "Light Assist";
      tech->category = TechCategory::ASSIST;
      tech->desc = 
        "HP COST: 6 | CD: 10s\n"
        "SCALING: Offense & Dexterity\n\n"

        "Xander does a little twirl while whipping his\n"
        "tail in the direction that Mary is facing.\n"
        "If Mary is too far away, Xander will instead\n"
        "try to move closer to his position before\n"
        "proceeding with the action.\n\n"

        "When the attack begins, any unfortunate\n"
        "enemy who happens to be too close to Xander\n"
        "will essentially get bodychecked.";
      tech++;

      tech->name = "Meteor";
      tech->type = "Heavy Assists";
      tech->category = TechCategory::ASSIST;
      tech->desc = 
        "HP COST: 8 | CD: 10s\n"
        "SCALING: Offense\n\n"

        "Xander raises a single fist, winds it back,\n"
        "and then proceed to strike down as hard as\n"
        "he can. It's exactly as deadly as it sounds.\n\n"

        "Easily capable of breaking the bones of\n"
        "the least resilient; If it hits that is.";
      tech++;

      tech->name = "Draconian";
      tech->type = "Passive Trait";
      tech->category = TechCategory::PASSIVE;
      tech->desc = 
        "Xander has an innate stun immunity to all\n"
        "Morale Attacks. As a trade off, any Morale\n"
        "damage Xander sustains will increase his\n"
        "Entropy instead.\n\n"

        "This passive will cease to function if Xander\n"
        "gets staggered, or if he's already in hitstun.";
      tech++;

      tech->name = "Protective";
      tech->type = "Passive Trait";
      tech->category = TechCategory::PASSIVE;
      tech->desc = 
        "For as long as Xander has Tenacity, most\n"
        "attacks will prioritize him over anyone else.\n"
        "Xander will do anything to protect his friend\n"
        "from serious injury and he doesn't like to be\n"
        "too far away from him either.\n\n"

        "On most occasions, the Water Dragon will\n"
        "often drop whatever he's doing, and rush\n"
        "towards Mary's position to reunite with him.";
      break;
    }
  }

  for (int x = 0; x < 6; x++) {
    TechInfo *tech = &tech_info.at(x);
    if (tech->category == TechCategory::NONE) {
      TechOption option = tech_options.at(x);
      tech_disallowed.emplace(option);
    }
  }
}

void TechsPanel::setupWeaponTech(SubWeaponID weapon_id, 
                                 array<TechInfo, 6>::iterator &tech)
{
  assert(tech != tech_info.end());
  switch (weapon_id) {
    case SubWeaponID::KNIFE: {
      tech->name = "Cleave";
      tech->type = "Light Technique";
      tech->category = TechCategory::SUB_WEAPON;
      tech->desc = 
        "MP COST: 6\n"
        "SCALING: Offense & Dexterity\n\n"

        "Mary performs a slash attack that will inflict\n"
        "damage to the first enemy it hits. The action\n"
        "itself has a shorter windup if canceled into.";

      tech++;
      assert(tech != tech_info.end());

      tech->name = "Piercer";
      tech->type = "Heavy Technique";
      tech->category = TechCategory::SUB_WEAPON;
      tech->desc = 
        "MP COST: 8\n"
        "SCALING: Offense & Dexterity\n\n"

        "Mary thrusts his knife in the direction he's\n"
        "facing. The attack is capable of piercing\n"
        "multiple enemies, and once it hits, Mary will\n"
        "brutally yank out of knife afterwards;\n"
        "Dealing even more damage to every enemy who\n"
        "got pierced by the initial attack.";
      break;
    }
    case SubWeaponID::BAT: {
      tech->name = "Swing";
      tech->type = "Light Technique";
      tech->category = TechCategory::SUB_WEAPON;
      tech->desc = 
        "MP COST: 5 | CD: 5s (On Occasion)\n"
        "SCALING: Offense\n\n"

        "Mary winds back his baseball bat, and swing\n"
        "as hard as he can. The attack is great in\n"
        "power, but slow in terms of speed.\n\n"
        "During the action's windup, Mary is able to\n"
        "freely change the direction he's facing.";

      tech++;
      assert(tech != tech_info.end());

      tech->name = "Hail Mary";
      tech->type = "Heavy Technique";
      tech->category = TechCategory::SUB_WEAPON;
      tech->desc = 
        "MP COST: 3\n"
        "SCALING: Offense & Dexterity\n\n"

        "Mary tosses a baseball high into the air. If\n"
        "the ball gets hit on its way down, most notably\n"
        "with Swing, then it will be launched forwards.\n"
        "The projectile itself is actually quite fast,\n"
        "and it's capable of causing a lot of damage\n"
        "when used correctly.";
      break;
    }
  }
}

string TechsPanel::getCategoryName(TechCategory category) {
  switch (category) {
    case TechCategory::SUB_WEAPON: {
      return "Sub-Weapon";
    }
    case TechCategory::ASSIST: {
      return "Assist";
    }
    case TechCategory::ACTION: {
      return "Action";
    }
    case TechCategory::PASSIVE: {
      return "Passive";
    }
    default: {
      assert(category != TechCategory::NONE);
      return "INVALID";
    }
  }
}

Color TechsPanel::getPrimaryColor(TechCategory category) {
  switch (category) {
    case TechCategory::SUB_WEAPON: {
      return Game::palette[40];
    }
    case TechCategory::ASSIST: {
      return Game::palette[49];
    }
    case TechCategory::ACTION: {
      return Game::palette[48];
    }
    case TechCategory::PASSIVE: {
      return Game::palette[13];
    }
    default: {
      assert(category != TechCategory::NONE);
      return BLACK;
    }
  }
}

Color TechsPanel::getSecondaryColor(TechCategory category) {
  switch (category) {
    case TechCategory::SUB_WEAPON: {
      return Game::palette[42];
    }
    case TechCategory::ASSIST: {
      return Game::palette[51];
    }
    case TechCategory::ACTION: {
      return Game::palette[50];
    }
    case TechCategory::PASSIVE: {
      return Game::palette[14];
    }
    default: {
      assert(category != TechCategory::NONE);
      return WHITE;
    }
  }
}

void TechsPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
    return;
  }

  switch (menu_state) {
    case PARTY_MEMBER: {
      blink_clock += Game::deltaTime();
      memberNavigation();
      break;
    }
    case TECH_SELECT: {
      blink_clock += Game::deltaTime();
      techNavigation();
      break;
    }
    case PANEL: {
      assert(panel != nullptr);
      panel->update();

      if (panel->terminate) {
        *description = "Select an Technique to read\n"
                       "information about it";
        menu_state = TECH_SELECT;
        panel.reset();
      }
      break;
    }
  }
}

void TechsPanel::heightLerp() {
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }
}

void TechsPanel::memberNavigation() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(options, selected);
    setupTech();

    sfx->play("menu_navigate");
    blink_clock = 0.0;
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected);
    setupTech();

    sfx->play("menu_navigate");
    blink_clock = 0.0;
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    *description = "Select an Technique to read\n"
                   "information about it";
    *desc_color = Game::palette[22];
    menu_state = TECH_SELECT;
    tech_selected = tech_options.begin();

    blink_clock = 0.0;
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void TechsPanel::techNavigation() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(tech_options, tech_selected, &tech_disallowed);
    sfx->play("menu_navigate");
    blink_clock = 0.0;
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(tech_options, tech_selected, &tech_disallowed);
    sfx->play("menu_navigate");
    blink_clock = 0.0;
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    *description = "";
    createTextbox();
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    *description = "Select a Party Member";
    *desc_color = WHITE;

    menu_state = PARTY_MEMBER;
    blink_clock = 0.0;
    sfx->play("menu_cancel");
  }
}

void TechsPanel::createTextbox() {
  assert(panel == nullptr && menu_state != PANEL);
  int index = static_cast<int>(*tech_selected);
  TechInfo *tech = &tech_info.at(index);

  Vector2 position = {101, 44};
  panel = make_unique<TextBoxPanel>(position, tech->name, tech->type,
                                    tech->desc);
  menu_state = PANEL;
}

void TechsPanel::draw() {
  drawOptions();
  drawTech();

  if (menu_state == PANEL) {
    assert(panel != nullptr);
    panel->draw();
  }
}

void TechsPanel::drawOptions() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Rectangle sprite = camp_atlas->sprites[0];
  if (state != PanelState::READY) {
    sprite.width *= percentage;
  }

  for (int index = 0; index < 2; index++) {
    Character *party_member = options[index];
    PartyMemberID id = party_member->member_id;

    Vector2 position = option_position;
    position.y += 16 * index;

    DrawTextureRec(camp_atlas->sheet, sprite, position, WHITE);

    Character *current = *selected;
    if (current->member_id == id) {
      drawCursor(position, menu_state == PARTY_MEMBER);
    }

    position = Vector2Add(position, {6, 1});
    DrawTextEx(*font, party_member->name, position, txt_size, -2, WHITE);
  }
}

void TechsPanel::drawCursor(Vector2 position, bool blink) {
  if (state != PanelState::READY) {
    return;
  }

  Rectangle *sprite = &menu_atlas->sprites[0];
  position = Vector2Add(position, {-13, 3});

  Color color = WHITE;

  if (blink) {
    float sin_a = std::sinf(blink_clock * 2.5);
    sin_a = (sin_a / 2) + 0.5;
    color.a = 255 * sin_a;
  }

  DrawTextureRec(menu_atlas->sheet, *sprite, position, color);
}

void TechsPanel::drawTech() {
  Vector2 position = tech_position;
  Color color;
  TechCategory current_category = TechCategory::NONE;

  for (TechOption &option : tech_options) {
    int index = static_cast<int>(option);
    TechInfo *tech = &tech_info.at(index);

    if (tech->category == TechCategory::NONE) {
      continue;
    }

    if (current_category != tech->category) {
      current_category = tech->category;
      color = getPrimaryColor(current_category);
      drawTechCategory(current_category, position, color);
      position.y += 16;
    }

    DrawTextureRec(camp_atlas->sheet, camp_atlas->sprites[11], position,
                   color);

    Vector2 txt_position = Vector2Add(position, {9, 1});
    DrawTextEx(*med_font, tech->name.c_str(), txt_position, med_size, -2,
               WHITE);

    if (menu_state == TECH_SELECT && tech_selected == &option) {
      drawCursor(position, true);
    }

    position.y += 16;
  }
}

void TechsPanel::drawTechCategory(TechCategory category, Vector2 position,
                                  Color frame_color)
{
  position.x -= 50;
  DrawTextureRec(camp_atlas->sheet, camp_atlas->sprites[10], position,
                 frame_color);

  Color txt_color = getSecondaryColor(category);
  string text = getCategoryName(category);
  Vector2 txt_position = Vector2Add(position, {45, 1});
  txt_position = TextUtils::alignCenter(text.c_str(), txt_position, 
                                        *med_font, -2, 0);
  DrawTextEx(*med_font, text.c_str(), txt_position, med_size, -2, 
             txt_color);
}
