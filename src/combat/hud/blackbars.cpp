#include <cassert>
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "data/combatant_event.h"
#include "utils/math.h"
#include "combat/hud/blackbars.h"
#include <plog/Log.h>

using std::unique_ptr;


BlackBars::BlackBars(float y_offset, bool alternate) {
  if (!alternate) {
    texture = LoadTexture("graphics/hud/blackbar.png");
  }
  else {
    texture = LoadTexture("graphics/hud/blackbar2.png"); 
  }
  tint.a = 225;

  this->y_offset = y_offset;
  top_dest.y -= y_offset;
  bottom_dest.y -= y_offset;
}

BlackBars::~BlackBars() {
  UnloadTexture(texture);
}

void BlackBars::setTint(Color tint) {
  this->tint = tint;
  this->tint.a = 225;
}

void BlackBars::setValues(float speed, float zoom) {
  set_speed = speed;
  this->speed = set_speed;
  decel_clock = 0.0;
  PLOGD << "Speed has been changed to: " << speed;

  if (zoom != 0) {
    set_zoom = zoom;
    this->zoom = zoom;
    zoom_clock = 0.0;
    PLOGD << "Zoom has been changed to: " << zoom;
  }
}

void BlackBars::setTargetValues(float speed, float zoom, float duration) {
  target_speed = speed;
  target_zoom = zoom;

  target_clock = 0.0;
  target_time = duration;

  set_speed = this->speed;
  set_zoom = this->zoom;

  decel_clock = 0.0;
  zoom_clock = 0.0;

  PLOGD << "Target speed and zoom has been changed to: " << target_speed
  << ", " << target_zoom;
  PLOGD << "Duration: " << target_time;
}

void BlackBars::resetTargetValues() {
  target_speed = DEFAULT_SPEED;
  target_zoom = DEFAULT_ZOOM;

  set_speed = speed;
  set_zoom = zoom;

  decel_clock = 0.0;
  zoom_clock = 0.0;
  PLOGD << "Target speed and zoom has been reset to their default values";
}

void BlackBars::evaluateEvent(unique_ptr<CombatantEvent> &event) {
  if (event->event_type != CombatantEVT::TOOK_DAMAGE) {
    return;
  }

  TookDamageCBT *dmg_event = static_cast<TookDamageCBT*>(event.get());
  if (dmg_event->damage_type != DamageType::LIFE) {
    return;
  }

  assert(dmg_event->sender->entity_type == COMBATANT);
  Combatant *victim = static_cast<Combatant*>(dmg_event->sender);

  float multiplier = 1;
  if (dmg_event->stun_type == StunType::STAGGER) {
    multiplier = 3;
  }

  bool from_enemy = victim->team == CombatantTeam::ENEMY;
  if (from_enemy) {
    setValues(90, 8 * multiplier);
    return;
  }

  PartyMember *member = static_cast<PartyMember*>(victim);
  if (member->important) {
    setValues(0, -8 * multiplier);
  }
}

void BlackBars::update(Camera2D *camera) {
  if (!enabled) {
    return;
  }

  top_dest.x = camera->target.x;
  top_dest.y = (24 - y_offset) + camera->target.y;
  bottom_dest.x = camera->target.x;
  bottom_dest.y = (216 - y_offset) + camera->target.y;

  if (speed != target_speed) {
    speedLerp();
  }

  if (zoom != target_zoom) {
    zoomLerp();
  }

  if (target_speed != DEFAULT_SPEED || target_zoom != DEFAULT_ZOOM) {
    targetTimer();
  }
}

void BlackBars::speedLerp() {
  assert(speed != target_speed);
  decel_clock += Game::deltaTime() / decel_time;
  decel_clock = Clamp(decel_clock, 0.0, 1.0);

  speed = Math::smoothstep(set_speed, target_speed, decel_clock);

  if (decel_clock == 1.0) {
    decel_clock = 0.0;
  }
}

void BlackBars::zoomLerp() {
  assert(zoom != target_zoom);
  zoom_clock += Game::deltaTime() / zoom_time;
  zoom_clock = Clamp(zoom_clock, 0.0, 1.0);

  zoom = Math::smoothstep(set_zoom, target_zoom, zoom_clock);

  if (zoom_clock == 1.0) {
    zoom_clock = 0.0;
  }
}

void BlackBars::targetTimer() {
  if (target_time < 0) {
    return;
  }

  target_clock += Game::deltaTime() / target_time;
  if (target_clock >= 1.0) {
    resetTargetValues();
  }
}

void BlackBars::draw() {
  if (!enabled) {
    return;
  }

  source.x += speed * Game::deltaTime();

  if (source.x > 16) {
    source.x = source.x - 16;
  }

  DrawTexturePro(texture, source, top_dest, {229, zoom}, 178, tint);
  DrawTexturePro(texture, source, bottom_dest, {229, zoom}, -2, tint);
}

