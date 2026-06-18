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

  void applySupplyPenalty(Session *session);
  void applyCompanionDamage(Companion *companion);

  void setupIncapTexture(CompanionID id);
  std::string getIncapMessage();

  void followUpSequence();
  void endSequence();

  void update() override;
  void draw() override;
private:
  Texture texture;
  Vector2 position;
  Color tint;

  std::string text;
  Color text_color;

  Session *session;


  float seq_clock = 0.0;
  float seq_time = 3.0;
};
