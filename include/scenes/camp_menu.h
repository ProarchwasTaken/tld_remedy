#pragma once
#include "base/scene.h"
#include "data/session.h"
#include "data/keybinds.h"


class CampMenuScene : public Scene {
public:
  CampMenuScene(Session *session);
  ~CampMenuScene();

  void update() override;
  void draw() override;
private:
  Session *session;
  MenuKeybinds *keybinds;
};
