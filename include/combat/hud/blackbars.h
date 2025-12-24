#pragma once
#include <memory>
#include <raylib.h>
#include "data/combatant_event.h"
#include "combat/system/camera.h"


class BlackBars {
public:
  BlackBars();
  ~BlackBars();

  void setValues(float speed, float zoom);
  void setTargetValues(float speed, float zoom, float duration = -1);
  void resetTargetValues();

  void evaluateEvent(std::unique_ptr<CombatantEvent> &event);
  void update(CombatCamera *camera);

  void speedLerp();
  void zoomLerp();

  void draw();
private:
  Vector2 top_position = {213, 24};
  Vector2 bottom_position = {213, 216};

  Texture texture;
  Color tint = WHITE;
  Rectangle source = {0, 0, 430, 80};

  Rectangle top_dest = {0, 24, 430, 80};
  Rectangle bottom_dest = {0, 216, 430, 80};

  static constexpr float DEFAULT_SPEED = 10;
  float speed = DEFAULT_SPEED;
  float set_speed = speed;
  float target_speed = speed;

  float decel_clock = 0.0;
  float decel_time = 0.8;

  static constexpr float DEFAULT_ZOOM = 0;
  float zoom = DEFAULT_ZOOM;
  float set_zoom = zoom;
  float target_zoom = zoom;

  float zoom_clock = 0.0;
  float zoom_time = 0.8;

  float target_clock = 0.0;
  float target_time = -1;
};
