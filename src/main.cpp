#include "Charlie2D.h"
#include <cmath>
#include <format>
#include <sstream>
#include <string>

#include "Components.h"
#include "Config.h"
#include "Creaters.h"
#include "toBeEnginized.h"

void loadMenuScene() {
  for (Entity *entity : GameManager::getAllObjects()) {
    entity->toDestroy = true;
  }

  Camera::setPosition({0, 0});

  createBackground({-256 / 2.0f, -256 / 2.0f, 256, 256});

  Entity *canvas = GameManager::createEntity("canvas");
  canvas->add<UICanvas>();

  createPlayButton(canvas);

  Entity *infoPanel = Templates::createUIPanel(
      canvas, {0, 0, 256 * 3, 500}, "Aim and fire cannons with Mouse\n\n \
      Mouse distance from cannon = Fire force\n\nSpace to reset Gift\n\nR to reset GAME");

  infoPanel->get<entityBox>()->setLocalWithCenter({0, 0});
}

void loadEndScene() {
  for (Entity *entity : GameManager::getAllObjects()) {
    entity->toDestroy = true;
  }

  Camera::setPosition({0, 0});

  createBackground({0, 0, 256, 256});

  Entity *canvas = GameManager::createEntity("canvas");
  canvas->add<UICanvas>();

  createPlayButton(canvas);

  float runTime = (SDL_GetTicks() - runTimerStart) / 1000.0f;
  runTime = std::round(runTime * 100.0f) / 100.0f;

  Entity *infoPanel = Templates::createUIPanel(
      canvas, {0, 0, 256 * 3, 400},
      "The End!\n\nThanks for playing\n\nTime: " + floatToString(runTime) +
          " seconds!");
  infoPanel->get<entityBox>()->setLocalWithCenter({0, 0});
}

void addLDTKFields(Entity *entity) {
  json fields = entity->get<LDTKEntity>()->entityJson["fieldInstances"];
  if (fields.size() <= 0)
    return;
  for (json field : fields) {
    if (field["__identifier"] == "movePoint" && !field["__value"].is_null()) {
      entity->add<MovePoint>()->startPoint =
          entity->get<entityBox>()->getBox().getCenter();

      Vector2f end = {field["__value"]["cx"], field["__value"]["cy"]};
      entity->add<MovePoint>()->endPoint = end * 16 + 8;
    } else if (field["__identifier"] == "moveSpeed") {
      entity->add<MovePoint>()->speed = (float)field["__value"];
    }

    if (field["__identifier"] == "movePoint" && field["__value"].is_null()) {
      entity->add<MovePoint>()->disable = true; 
    }
  }
}

void loadGameScene() {
  runTimerStart = SDL_GetTicks();
  GameManager::playSound(CANNON_FIRE_SOUND);
  for (Entity *entity : GameManager::getAllObjects()) {
    entity->toDestroy = true;
  }

  Entity *canvas = GameManager::createEntity("canvas");
  canvas->add<UICanvas>();

  Entity *timerPanel = Templates::createUIPanel(canvas, {0, 0, 64 * 4, 15 * 4},
                                                "TIMER", "timerPanel");
  timerPanel->get<entityBox>()->anchor = 1;
  timerPanel->get<entityBox>()->setLocalWithCenter({0, 16 * 2});

  Entity *god = GameManager::createEntity("God");
  god->add<God>();
  god->get<God>()->timerPanel = timerPanel;

  LDTK::onLoadLevel = []() {
    Camera::setPosition({LDTK::worldBox.position.x + 256 * 4 / 2.0f,
                         LDTK::worldBox.position.y + 256 * 4 / 2.0f});

    createBackground(LDTK::worldBox);

    for (Entity *entity : GameManager::getAllObjects()) {
      if (entity->toDestroy)
        continue;
      if (entity->tag == "Ground") {
        entity->add<Collider>()->solid = true;
      } else if (entity->tag == "Tree") {
        entity->add<Sprite>()->loadTexture("img/tree.png", false);
      } else if (entity->tag == "Cannon") {
        entity->add<Sprite>()->loadTexture("img/cannon.png");
        entity->add<Cannon>();
        addLDTKFields(entity);
      } else if (entity->tag == "GiftSpawn") {
        GameManager::getEntities("God")[0]->get<God>()->reset();
      } else if (entity->tag == "Star") {
        entity->add<Sprite>()->loadTexture("img/star.png");
        entity->get<entityBox>()->setScale({14, 14});
        addLDTKFields(entity);
      } else if (entity->tag == "Portal") {
        json field = entity->get<LDTKEntity>()->entityJson["fieldInstances"][0];
        addLDTKFields(entity);
        Vector2f BPortalPosition = Vector2f(field["__value"]["cx"], field["__value"]["cy"]) * 16; 
        createPortal(entity, entity->get<entityBox>()->getPosition(), BPortalPosition);
      } else if (entity->tag == "Slime") {
        entity->add<Sprite>()->loadTexture("img/Slice.png");
        entity->add<Slime>();
        addLDTKFields(entity);
      } else if (entity->tag == "Glime") {
        entity->add<Sprite>()->loadTexture("img/Glime.png");
        entity->add<Glime>();
        addLDTKFields(entity);
      }
    }
  };

  LDTK::loadLevel(LDTK::fullJSON["worlds"][currentWorld]["levels"][0]["iid"], false);
}

int main() {
  GameManager::init({256 * 4, 256 * 4});
  Camera::scale = 4;
  LDTK::loadJson("img/ldtk/ldtk.ldtk");

  GameManager::playSound(TREE_HIT_SOUND);
  Mix_Music *music = Mix_LoadMUS(BACKGROUND_MUSIC.c_str());
  Mix_PlayMusic(music, -1);

  loadMenuScene();

  GameManager::doUpdateLoop();

  Mix_FreeMusic(music);

  return 0;
}
