#pragma once
#include "base/scene.h"
#include "data/session.h"


class CampMenuScene : public Scene {
public:
  CampMenuScene(Session *session);

  void update() override;
  void draw() override;
private:
  Session *session;
};
