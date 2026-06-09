#pragma once
#include <raylib.h>
#include <string>
#include "enums.h"
#include "base/field_sequence.h"
#include "data/session.h"



class IncapSequence : public FieldSequence {
public:
  IncapSequence(Session *session);
  ~IncapSequence();

  void setupIncapTexture(CompanionID id);

  void update() override;
  void draw() override;
private:
  Texture texture;
  Vector2 position;
  Color tint;

  float seq_clock = 0.0;
  float seq_time = 3.0;
};
