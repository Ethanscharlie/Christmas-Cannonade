#pragma once
#include "Charlie2D.h"
#include "Component.h"
#include "Config.h"
#include "Entity.h"
#include "EntityBox.h"
#include "GameManager.h"
#include "Scheduler.h"
#include "Vector2f.h"
#include "imguiUtils.h"
#include "toBeEnginized.h"
#include <cmath>

class God;
void loadEndScene();
void loadGameScene();
Entity *createGift(Vector2f position, Vector2f direction, float force,
                   Entity *firedFrom);

class Gift : public Component {
public:
  void update(float deltaTime) override {
    Camera::setPosition(entity->get<entityBox>()->getBox().getCenter());

    Entity *tree = GameManager::getEntities("Tree")[0];
    if (entity->get<entityBox>()->getBox().checkCollision(
            tree->get<entityBox>()->getBox())) {

      if (LDTK::fullJSON["worlds"][currentWorld]["levels"].size() - 1 >
          currentLevel) {
        currentLevel++;
      } else {
        currentLevel = 0;
        loadEndScene();
        return;
      }

      for (Entity *entityO : GameManager::getAllObjects()) {
        if (entityO->checkComponent<God>())
          continue;
        if (entityO->tag == "canvas")
          continue;
        if (entityO->tag == "timerPanel") {
          std::cout << "Found" << std::endl;
          continue;
        }
        if (entityO->tag == "Snowflake") {
          continue;
        }
        entityO->toDestroy = true;
      }

      GameManager::playSound(TREE_HIT_SOUND);

      LDTK::loadLevel(
          LDTK::fullJSON["worlds"][currentWorld]["levels"][currentLevel]["iid"],
          false);
    }

    checkKill("Kill");
    checkKill("Star");
  }

  void checkKill(std::string tag) {
    for (Entity *kill : GameManager::getEntities(tag)) {
      if (entity->get<entityBox>()->getBox().checkCollision(
              kill->get<entityBox>()->getBox())) {
        entity->toDestroy = true;
        GameManager::playSound(HIT_SPIKE_SOUND);

        createGift(GameManager::getEntities("GiftSpawn")
                       .back()
                       ->get<entityBox>()
                       ->getBox()
                       .getCenter(),
                   {0, 0}, 0, nullptr);
      }
    }
  }

  Entity *firedFrom = nullptr;
};

class Gravity : public Component {
public:
  void start() override { entity->add<physicsBody>(); }

  void update(float deltaTime) override {

    entity->get<physicsBody>()->velocity.y += gravity * deltaTime;
  }

  float gravity = 1500;
};

Entity *createGift(Vector2f position, Vector2f direction, float force,
                   Entity *firedFrom);

class Cannon : public Component {
public:
  void start() override {}

  void update(float deltaTime) override {
    if (hasGift) {
      Camera::setPosition(entity->get<entityBox>()->getBox().getCenter());
      rotation.lookAt(entity->get<entityBox>()->getBox().getCenter(),
                      InputManager::getMouseWorldPosition());
      entity->get<Sprite>()->angle = rotation;

      if (InputManager::checkInput("fire")) {
        fire();
        unarmedState();
      }

      float mouseDistance = entity->get<entityBox>()->getBox().getCenter().dist(
          InputManager::getMouseWorldPosition());
      float force = forceMultiplier * mouseDistance;
      if (force > maxForce) {
        force = maxForce;
      } else if (force < 300) {
        force = 300;
      }

      entity->get<entityBox>()->setScale({force / 10, 16});
      // entity->get<entityBox>()->setScale({48, 16});
    } else {
      entity->get<entityBox>()->setScale({48, 16});
    }

    if (GameManager::getEntities("Gift").size() > 0) {
      Entity *gift = GameManager::getEntities("Gift")[0];
      if (entity->get<entityBox>()->getBox().checkCollision(
              gift->get<entityBox>()->getBox()) &&
          gift->get<Gift>()->firedFrom != entity && !gift->toDestroy) {
        gift->toDestroy = true;
        hasGift = true;

        entity->get<Sprite>()->loadTexture(armedTexture, false);
      }
    }
  }

  void unarmedState() {
    hasGift = false;
    entity->get<Sprite>()->loadTexture(defaultTexture, false);
  }

  void fire() {
    float mouseDistance = entity->get<entityBox>()->getBox().getCenter().dist(
        InputManager::getMouseWorldPosition());
    float force = forceMultiplier * mouseDistance;
    if (force > maxForce) {
      force = maxForce;
    }

    GameManager::playSound(CANNON_FIRE_SOUND);

    createGift(entity->get<entityBox>()->getBox().getCenter(),
               rotation.getVector(), force, entity);
  }

  Angle rotation;
  bool hasGift = false;

  // const float forceMultiplier = 5;
  const float forceMultiplier = 8;
  // const float maxForce = 800;
  const float maxForce = 800;
  const std::string defaultTexture = "img/cannon.png";
  const std::string armedTexture = "img/armedCannon.png";
};

Entity *createSnowflake();

class Snowflake : public Component {
public:
  void start() override {}

  void update(float deltaTime) override {
    entity->get<entityBox>()->changePosition(move * deltaTime);
    if (!entity->get<entityBox>()->getBox().checkCollision(LDTK::worldBox)) {
      entity->toDestroy = true;
    }
  }

  Vector2f move = {100, 400};
};

class God : public Component {
public:
  void start() override {
    entity->add<Scheduler>()->addSchedule("snowflake", 15,
                                          []() { createSnowflake(); });
  }

  void update(float deltaTime) override {
    if (InputManager::checkInput("jumpTrigger")) {
      GameManager::playSound(GIFT_RESET_SOUND);
      reset();
    }

    if (InputManager::keys[SDLK_r]) {
      currentLevel = 0;
      loadGameScene();
    }

    if (timerPanel != nullptr) {
      float runTime = (SDL_GetTicks() - runTimerStart) / 1000.0f;
      runTime = std::round(runTime * 100.0f) / 100.0f;
      timerPanel->get<Text>()->text = floatToString(runTime);
    }
  }

  void reset() {
    for (Gift *gift : GameManager::getComponents<Gift>()) {
      gift->entity->toDestroy = true;
    }

    for (Cannon *cannon : GameManager::getComponents<Cannon>()) {
      cannon->unarmedState();
    }

    createGift(GameManager::getEntities("GiftSpawn")
                   .back()
                   ->get<entityBox>()
                   ->getBox()
                   .getCenter(),
               {0, 0}, 0, nullptr);
  }

  Entity *timerPanel = nullptr;
};

class StartMenuDetecter : public Component {
public:
  void update(float deltaTime) override {
    if (InputManager::checkInput("jump")) {
      loadGameScene();
    }
  }
};

class MovePoint : public Component {
public:
  void update(float deltaTime) override {
    if (disable)
      return;
    Vector2f currentPosition = entity->get<entityBox>()->getBox().getCenter();
    if (movingToEnd) {
      // Check Past
      Vector2f direction = (endPoint - currentPosition).normalize();
      entity->get<entityBox>()->changePosition(direction * speed * deltaTime);

      if (currentPosition.dist(endPoint) < lowSpeed)
        movingToEnd = false;
    } else {

      // Check Past
      Vector2f direction = (startPoint - currentPosition).normalize();
      entity->get<entityBox>()->changePosition(direction * speed * deltaTime);

      if (currentPosition.dist(startPoint) < lowSpeed)
        movingToEnd = true;
    }
  }

  bool movingToEnd = true;
  bool disable = false;

  float speed = 60.0f;
  Vector2f startPoint;
  Vector2f endPoint;

  const float lowSpeed = 8;
};

class Portal : public Component {
public:
  void start() override {}

  void update(float deltaTime) override {
    for (Gift *gift : GameManager::getComponents<Gift>()) {
      if (!entity->add<entityBox>()->getBox().checkCollision(
              gift->entity->get<entityBox>()->getBox()))
        continue;
      if (otherPortal == nullptr)
        continue;
      if (!readyForTeleport)
        continue;
      if (!otherPortal->readyForTeleport)
        continue;

      gift->entity->get<entityBox>()->setPosition(
          otherPortal->entity->get<entityBox>()->getPosition());
      readyForTeleport = false;
      entity->add<Scheduler>()->addSchedule(
          "PortalCooldown", 300, [this]() { readyForTeleport = true; }, true);
    }

    if (!entity->get<Sprite>()->animations["general"]->playing) {
      entity->get<Sprite>()->animations["general"]->play();
    }
  }

  bool readyForTeleport = true;
  Portal *otherPortal = nullptr;
};

class Slime : public Component {
public:
  void start() override {}

  void update(float deltaTime) override {
    for (Gift *gift : GameManager::getComponents<Gift>()) {
      if (!entity->add<entityBox>()->getBox().checkCollision(
              gift->entity->get<entityBox>()->getBox()))
        continue;
      if (!readyForTeleport)
        continue;

      gift->entity->get<physicsBody>()->velocity *= -1;
      readyForTeleport = false;
      entity->add<Scheduler>()->addSchedule(
          "PortalCooldown", 100, [this]() { readyForTeleport = true; }, true);
    }
  }

  bool readyForTeleport = true;
};

class Glime : public Component {
public:
  void start() override {}

  void update(float deltaTime) override {
    for (Gift *gift : GameManager::getComponents<Gift>()) {
      if (!entity->add<entityBox>()->getBox().checkCollision(
              gift->entity->get<entityBox>()->getBox()))
        continue;
      if (!readyForTeleport)
        continue;

      Vector2f velo = gift->entity->get<physicsBody>()->velocity;
      gift->entity->get<physicsBody>()->velocity = {velo.y, velo.x};
      gift->entity->get<entityBox>()->setPosition(
          entity->get<entityBox>()->getPosition());

      // if (velo.x <= 0 && velo.y <= 0) {
      //   GameManager::getComponents<God>()[0]->reset();
      // }

      readyForTeleport = false;
      entity->add<Scheduler>()->addSchedule(
          "PortalCooldown", 100, [this]() { readyForTeleport = true; }, true);
    }
  }

  bool readyForTeleport = true;
};
