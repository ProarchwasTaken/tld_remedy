#include <cassert>
#include <cstddef>
#include <random>
#include <raylib.h>
#include "game.h"
#include "base/combatant.h"
#include "base/ai_parameters.h"
#include "data/rect_ex.h"
#include <plog/Log.h>

using std::uniform_real_distribution;


void AIParameters::wait(float seconds) {
  wait_time = seconds;
  wait_clock = 0.0;
  waiting = true;
  PLOGI << "Waiting for: " << seconds << " seconds.";
}

void AIParameters::wait(float min_seconds, float max_seconds) {
  uniform_real_distribution<float> range(min_seconds, max_seconds);
  wait_time = range(Game::RNG);
  wait_clock = 0.0;
  waiting = true;
  PLOGI << "Waiting for: " << wait_time << " seconds.";
}

void AIParameters::waitTimer() {
  wait_clock += Game::deltaTime() / wait_time;

  if (wait_clock >= 1.0) {
    wait_clock = 0.0;
    waiting = false;
  }
}


bool AIParameters::inAttackRange(Combatant *owner, Combatant *target) {
  assert(target != NULL);
  float distance = owner->distanceTo(target);
  float half_scale = target->hurtbox.scale.x / 2;

  distance = distance - half_scale;
  return distance <= attack_distance;
}

void AIParameters::drawDebug(int ai_goal, Vector2 position, 
                             RectEx &bounding_box) 
{
  drawDist(position, contest_distance, YELLOW);
  drawDist(position, attack_distance, RED);
  drawTimers(ai_goal, bounding_box.position);
}

void AIParameters::drawDist(Vector2 position, float distance, Color color)
{
  position.y += 2;

  Vector2 start1 = position;
  start1.x -= distance;

  Vector2 end1 = position;
  end1.x += distance;

  Vector2 start2 = end1;
  start2.y -= 8;

  Vector2 end2 = start1;
  end2.y -= 8;

  DrawLineV(start1, end1, color);
  DrawLineV(start1, end2, color);
  DrawLineV(end1, start2, color);
}

void AIParameters::drawTimers(int ai_goal, Vector2 position) {
  Font *font = &Game::sm_font;
  int size = font->baseSize;

  const char *txt_goal = TextFormat("%i", ai_goal);
  DrawTextEx(*font, txt_goal, position, size, -3, RED);

  if (cooldown_clock < 1.0) {
    position.y -= 8;
    float percentage = 1.0 - cooldown_clock;
    float time_left = attack_cooldown * percentage;

    const char *text = TextFormat("%00.02f", time_left);
    DrawTextEx(*font, text, position, size, -3, PINK);
  }

  if (waiting) {
    position.y -= 8;
    float percentage = 1.0 - wait_clock;
    float time_left = wait_time * percentage;

    const char *text = TextFormat("%00.02f", time_left);
    DrawTextEx(*font, text, position, size, -3, YELLOW);
  }

  if (dodge_clock != 0.0 && dodge_clock < 1.0) {
    position.y -= 8;
    float percentage = 1.0 - dodge_clock;
    float time_left = dodge_time * percentage;

    const char *text = TextFormat("%00.02f", time_left);
    DrawTextEx(*font, text, position, size, -3, ORANGE);
  }

  if (retreat_clock != 0.0 && retreat_clock < 1.0) {
    position.y -= 8;
    float percentage = 1.0 - retreat_clock;
    float time_left = retreat_time * percentage;

    const char *text = TextFormat("%00.02f", time_left);
    DrawTextEx(*font, text, position, size, -3, BLUE);
  }
}
