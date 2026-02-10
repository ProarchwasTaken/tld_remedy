#pragma once
#include <memory>
#include <raylib.h>
#include "data/combatant_event.h"


class BlackBars {
public:
  BlackBars(float y_offset = 0, bool alternate = false);
  ~BlackBars();

  void setTint(Color tint);
  void setValues(float speed, float zoom);
  void setTargetValues(float speed, float zoom, float duration = -1);
  void resetTargetValues();

  void evaluateEvent(std::unique_ptr<CombatantEvent> &event);
  void update(Camera2D *camera);

  void speedLerp();
  void zoomLerp();
  void targetTimer();

  void draw();

  bool enabled = true;
private:
  Texture texture;
  Color tint = WHITE;
  Rectangle source = {0, 0, 458, 80};

  Rectangle top_dest = {0, 24, 458, 80};
  Rectangle bottom_dest = {0, 216, 458, 80};
  float y_offset;

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
