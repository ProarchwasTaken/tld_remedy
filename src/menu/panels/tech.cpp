#include <cassert>
#include <cstddef>
#include <string>
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
#include "menu/panels/tech.h"
#include <plog/Log.h>

using std::string;


TechsPanel::TechsPanel(Session *session, string *description, 
                       Color *desc_color) 
{
  id = PanelID::TECH;
  frame = LoadTexture("graphics/menu/frames/tech.png");

  assert(description != NULL);
  this->description = description;
  *description = "Press 'CONFIRM' to enable scrolling.";

  assert(desc_color != NULL);
  this->desc_color = desc_color;

  options[0] = &session->player;
  options[1] = &session->companion;

  selected = options.begin();
  updateStyleText();

  tech_canvas = LoadRenderTexture(219, 133);
  updateTechCanvas();

  menu_atlas = &Game::menu_atlas;
  menu_atlas->use();

  camp_atlas = &CampMenuScene::atlas;
  camp_atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();

  keybinds = &Game::settings.menu_keybinds;
  PLOGI << "Techs Panel: Initialized";
}

TechsPanel::~TechsPanel() {
  UnloadTexture(frame);
  UnloadRenderTexture(tech_canvas);
  menu_atlas->release();
  camp_atlas->release();
  sfx->release();
}

void TechsPanel::updateStyleText() {
  Character *party_member = *selected;
  switch (party_member->member_id) {
    case PartyMemberID::MARY: 
    case PartyMemberID::ERWIN: {
      style_text = "Weapon Techniques";
      break;
    }
    case PartyMemberID::XANDER: {
      style_text = "Animalistic";
      break;
    }
  }

  Font *font = &Game::med_font;
  style_position = TextUtils::alignRight(style_text.c_str(), {412, 45}, 
                                         *font, -2, 0);
}

string TechsPanel::weaponTechInfo(SubWeaponID id) {
  switch (id) {
    case SubWeaponID::KNIFE: {
      return 
      "<Cleave> - MP Cost: 8\n"
      "\"We all have to start somewhere.\"\n\n"

      "Mary performs a slash attack that inflicts\n"
      "an average amount of damage to the first\n"
      "enemy in front of him. The action itself also\n"
      "has a shorter windup if canceled into.\n\n"

      "<Piercer> - MP Cost: 12\n"
      "\"Commit when it counts. \"\n\n"

      "Mary thrusts his knife while propelling\n"
      "himself forward in the direction he's facing.\n"
      "It's an attack that's capable of piercing\n"
      "multiple enemies at once. If used successfuly,\n"
      "Mary will proceed to brutally yank out the\n"
      "knife, inflicting more damage to all enemies\n"
      "who got pierced by the initial attack.\n\n";
    }
    case SubWeaponID::BAT: {
      return
      "<Swing> - MP Cost: 6\n"
      "\"One decisive blow, and the battles over.\"\n\n"

      "Mary wind back his baseball bat, and swings\n"
      "as hard as he can. The attack is great in\n"
      "power, but slow in terms of speed. During the\n"
      "action's windup, Mary can freely change the\n"
      "direction he's currently facing.\n\n"

      "<Hail Mary> - MP Cost: 6\n"
      "\"When there's love, there's hope...\n"
      "but a little luck wouldn't hurt.\n\n"

      "Mary tosses a baseball into the air. On it's\n"
      "own, the projectile wouldn't inflict any\n"
      "damage if it were to hit an enemy. Rather,\n"
      "if Mary hits the baseball on it's way down\n"
      "with Swing, the projectile will be launched\n"
      "forward; Inflicting damage that scales with\n"
      "Mary's Offense and Dexterity.\n\n";
    }
  }
}

string TechsPanel::getTechEntry() {
  Character *party_member = *selected;

  switch (party_member->member_id) {
    case PartyMemberID::MARY: {
      Player *player = static_cast<Player*>(party_member);
      string text =
        "--[COMBATANT SUMMARY]--\n"
        "Mary has to play a very active role to make\n"
        "sure he and his companion survive unscathed.\n"
        "Having to guide his allies towards achieving\n"
        "their shared goal, the role of Leader is\n"
        "something that requires a great deal of\n"
        "diligence. Because the moment he falters is\n"
        "where everything falls apart.\n\n" 

        "--[WEAPON TECHNIQUES]--\n\n" +

        weaponTechInfo(player->weapon_id) +

        "--[DEFENSIVE ACTIONS]--\n\n"

        "<Ghost Step> - HP Cost: Depends on Dexterity\n"
        "Input: 'DEFENSIVE' + 'LEFT' / 'RIGHT'\n"
        "\"You can't afford be to be reckless.\"\n\n"

        "Mary dashes forwards or backwards\n"
        "depending on the input used. When\n"
        "performing the action, Mary is rendered\n"
        "intangible to any and all attacks. Only\n"
        "becoming vulnerable again once he enters end\n"
        "lag.\n\n"

        "If Mary is in the end lag of any action that\n"
        "isn't Ghost Step, he could cancel into it.\n"
        "Mary could also cancel into any action \n"
        "during the end lag of Ghost Step. Both of\n"
        "these acts are referred as Ghost Cancelling.\n\n"

        "<Evade> - HP Cost: Depends on Dexterity\n"
        "Input: 'DEFENSIVE' + 'DOWN'\n"
        "\"Save your energy, and play the long game.\"\n\n"

        "After a brief windup, Mary braces himself to\n"
        "dodge the first attack that comes his way.\n"
        "If successful, the damage Mary would've\n"
        "taken will instead be converted to Exhaustion\n"
        "that will deplete overtime.\n\n"

        "After a successful evade, Mary will also be\n"
        "rendered intangible to all attacks during end\n"
        "lag, along with being able to cancel Evade\n"
        "into any action.\n\n"

        "--[PASSIVES]--\n\n"

        "<Humanity>\n"
        "\"May your spirit always shine through.\"\n\n"

        "Mary's basic attack only inflicts Morale\n"
        "damage. In return, he gains Morale\n"
        "proportional to how much damage was dealt.\n"
        "If Mary isn't Despondent, a percentage of\n"
        "the Morale gained will be shared with all\n"
        "other party members.\n\n"

        "<Auto-Evade>\n"
        "\"All it takes is one hit, so you better learn\n"
        "how to DODGE.\"\n\n"

        "Mary automatically dodges most attacks at the\n"
        "cost of losing Morale. The amount lost is\n"
        "dependent on the victim's Persistence vs.\n"
        "the assailant's Intimidation.\n\n"

        "If Mary's Morale ends up below 0, he will\n"
        "become Despondent. During which,\n"
        "Auto-Evade becomes much easier to bypass.\n\n"
      ;
      return text;
    }
    case PartyMemberID::ERWIN: {
      string text = 
        "--[COMBATANT SUMMARY]--\n"

        "Erwin's overall gameplan is focused around\n"
        "relieving pressure rather than inflicting\n"
        "damage. He prefers to target enemies that\n"
        "Mary isn't already targeting,\n\n"

        "--[ASSISTS]--\n\n"

        "<Provoke> - MP Cost: 8 | CD: 5s\n"
        "\"Buying us a little more time. I trust that you\n"
        "can make every second count.\"\n\n"

        "Erwin performs a taunt that draws the aggro\n"
        "of all enemies that happen to be targeting\n"
        "Mary. There is a limit to how many enemies that\n"
        "can be provoked though. Erwin also receives\n"
        "intangibility that lasts until the end lag of\n"
        "the action.\n\n"

        "<3rd Party> - MP Cost: 10 | CD: 8s\n"
        "\"The notion of playing 'fair' when your life\n"
        "is on the line strikes me as foolish.\"\n\n"

        "Erwin runs to the enemy that Mary was\n"
        "targeting at the time of when the Assist was\n"
        "called. Once within range, Erwin will perform\n"
        "a dropkick that inflicts a high amount of\n"
        "knockback if it hits. Obviously, this Assist\n"
        "cannot be called if Mary isn't targeting\n"
        "anybody.\n\n"

        "--[PASSIVES]--\n\n"

        "<Humanity>\n"
        "\"May your spirit always shine through.\"\n\n"

        "Erwin's basic attack only inflicts Morale\n"
        "damage. In return, he gains Morale\n"
        "proportional to how much damage was dealt.\n"
        "If Erwin isn't Despondent, a percentage of\n"
        "the Morale gained will be shared with all\n"
        "other party members.\n\n"

        "<Auto-Evade>\n"
        "\"All it takes is one hit, so you better learn\n"
        "how to DODGE.\"\n\n"

        "Erwin automatically dodges most attacks at\n"
        "the cost of losing Morale. The amount lost is\n"
        "dependent on the victim's Persistence vs.\n"
        "the assailant's Intimidation.\n\n"

        "If Erwin's Morale ends up below 0, he will\n"
        "become Despondent. During which,\n"
        "Auto-Evade becomes much easier to bypass."
      ;
      return text;
    }
    case PartyMemberID::XANDER: {
      string text = 
        "--[COMBATANT SUMMARY]--\n"

        "Xander plays the self-proclaimed role of\n"
        "protector to Mary. As a dragon, he is capable\n"
        "of great feats of strength and durability,\n"
        "but he is prone to getting worn down through\n"
        "persistent effort.\n\n"
        
        "The years have not been kind to him. His body\n"
        "is becoming increasingly hard to sustain by\n"
        "the day. Make sure to provide help when he\n"
        "needs it.\n\n"

        "--[ASSISTS]--\n\n"

        "<Tail Whip> - HP Cost: 8 | CD: 5s\n"
        "\"Watch your tone.\"\n\n"

        "Xander will move in front of Mary and\n"
        "proceed to lash his tail with an incredible\n"
        "amount of force. This attack is capable of\n"
        "hitting multiple enemies at once, and it's\n"
        "damage also scales with Xander's Dexterity.\n\n"

        "If Xander is already in front of Mary, and\n"
        "is a considerable distance away at the time\n"
        "the assist was called, he will instead\n"
        "perform the attack in Mary's direction.\n\n"

        "<Steel Wall> - HP Cost: N/A | CD: 5s\n"
        "\"Get behind me...\"\n\n"

        "Xander will take on a defensive stance for\n"
        "half a second. While in this stance, Xander's\n"
        "assist cooldowns will be frozen and any\n"
        "damage he takes will be reduced by 50%.\n\n"

        "Xander also gains full Stun Immunity to\n"
        "all attacks.\n\n"

        "The stance timer resets every time Xander\n"
        "take damage, and it will be frozen for as\n"
        "long as Mary remains close to him.\n\n"

        "If Xander were to have no Tenacity. The\n"
        "stance will be broken the next time he\n"
        "take damage. The same applies to Critical\n"
        "Life as well.\n\n"

        "<Meteor> - HP Cost: 8 | CD: 10s\n"
        "\"Kneel.\"\n\n"

        "Xander will clasp his hands together, and\n"
        "and raise them over his head before.\n"
        "striking down. Very strong in terms of damage\n"
        "and stun time.\n\n"

        "This assist is only accessible while\n"
        "Steel Wall is active, with it temporarily\n"
        "replacing Tail Whip after one second.\n\n"


        "--[PASSIVES]--\n\n"

        "<Draconic>\n"
        "\"Old habits die hard.\"\n\n"

        "Xander does not possess Morale, nor can\n"
        "he become Despondent either. Instead, any\n"
        "Morale damage he sustains will increase\n"
        "his Entropy; Which drains his life overtime.\n\n"

        "As a trade off, Xander has an innate stun\n"
        "immunity to all Morale attacks. This effect\n"
        "will cease to function if Xander gets\n"
        "staggered, or he's already in hit stun.\n\n"

        "<Protective>\n"
        "\"Please remain close...\"\n\n"

        "Xander possesses Natural Tenacity;\n"
        "a secondary HP resource that absorbs a\n"
        "percentage of any Life damage he sustains.\n"
        "It will regenerate on its own if Xander\n"
        "doesn't take any damage for a certain\n"
        "period of time.\n\n"

        "For as long as Xander has Tenacity, most\n"
        "Single-Target attacks will prioritize him\n"
        "over Mary. No matter what.\n"
      ;

      return text;
    }
  }
}

void TechsPanel::updateTechCanvas() {
  string text = getTechEntry();
  Vector2 position = {0, -scroll_y};
  Font *font = &Game::sm_font;
  int txt_size = font->baseSize;

  float max_height = tech_canvas.texture.height;
  float height = MeasureTextEx(*font, text.c_str(), txt_size, -3).y;

  if (height > max_height) {
    max_scroll = height - max_height;
  }
  else {
    max_scroll = 0;
  }

  SetTextLineSpacing(12);
  BeginTextureMode(tech_canvas);
  {
    ClearBackground(BLACK);
    DrawTextEx(*font, text.c_str(), position, txt_size, -3, WHITE);
  }
  EndTextureMode();
  SetTextLineSpacing(16);
}

void TechsPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
    return;
  }

  if (!scrolling_mode) {
    blink_clock += Game::deltaTime();
    optionNavigation();
  }
  else {
    scrollingInput();
  }
}

void TechsPanel::heightLerp() {
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 152 * percentage;
}

void TechsPanel::optionNavigation() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(options, selected);
    updateStyleText();
    updateTechCanvas();

    sfx->play("menu_navigate");
    scroll_y = 0;
    blink_clock = 0.0;
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected);
    updateStyleText();
    updateTechCanvas();

    sfx->play("menu_navigate");
    scroll_y = 0;
    blink_clock = 0.0;
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    *description = "Press 'CANCEL' to disable scrolling.";
    *desc_color = Game::palette[22];

    scrolling_mode = true;
    blink_clock = 1.0;
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void TechsPanel::scrollingInput() {
  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->cancel, gamepad)) {
    *description = "Press 'CONFIRM' to enable scrolling.";
    *desc_color = WHITE;

    scrolling_mode = false;
    sfx->play("menu_cancel");
    return;
  }

  if (max_scroll <= 0) {
    return;
  }

  bool down = Input::down(keybinds->down, gamepad);
  bool up = Input::down(keybinds->up, gamepad);
  int direction = down - up;

  if (direction != 0) {
    scroll_y += (scroll_speed * direction) * Game::deltaTime();
    scroll_y = Clamp(scroll_y, 0.0, max_scroll);
    updateTechCanvas();
  }
}

void TechsPanel::draw() {
  Rectangle source = {0, 0, 224, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);

  if (state == PanelState::READY) {
    Font *font = &Game::med_font;
    int txt_size = font->baseSize;
    DrawTextEx(*font, style_text.c_str(), style_position, txt_size, -2, 
               WHITE);

    DrawTextureRec(tech_canvas.texture, canvas_source, {194, 60}, WHITE);
  }

  drawOptions();
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
      drawCursor(position);
    }

    position = Vector2Add(position, {6, 1});
    DrawTextEx(*font, party_member->name, position, txt_size, -2, WHITE);
  }
}

void TechsPanel::drawCursor(Vector2 position) {
  if (state != PanelState::READY) {
    return;
  }

  Rectangle *sprite = &menu_atlas->sprites[0];
  position = Vector2Add(position, {-13, 3});

  Color color = WHITE;
  float sin_a = std::sinf(blink_clock * 2.5);
  sin_a = (sin_a / 2) + 0.5;
  color.a = 255 * sin_a;

  DrawTextureRec(menu_atlas->sheet, *sprite, position, color);
}
