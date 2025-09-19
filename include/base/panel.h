#pragma once
#include "enums.h"


enum class PanelState {
  READY,
  OPENING,
  CLOSING
};


class Panel {
public:
  virtual ~Panel() = default;

  virtual void update() = 0;
  void transitionLogic();

  virtual void draw() = 0;

  PanelID id;
  bool terminate = false;
protected:
  PanelState state = PanelState::OPENING;
  float clock = 0.0;
  float transition_time = 0.25;
};
