#pragma once
#include <string>
#include <raylib.h>
#include "base/scene.h"
#include "data/session.h"


class RestMenuScene : public Scene {
public:
  RestMenuScene(Session *session);
  ~RestMenuScene();

  void loadBackground(std::string location);

  // void update() override;
  // void draw() override;
private:
  Session *session;
  Texture background;
  Color bg_color;
};
