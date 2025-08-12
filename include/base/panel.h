#pragma once
#include "enums.h"


class Panel {
public:
  virtual ~Panel() = default;

  virtual void update() = 0;
  virtual void draw() = 0;

  PanelID id;
  bool terminate = false;
};
