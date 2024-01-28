/**
 * @file
 * @brief Includes necessary libraries and declares several functions for the
 * creation of game entities.
 */

#include "Charlie2D.h"
#include "Components.h"
#include "EntityBox.h"
#include "GameManager.h"
#include "Math.h"
#include "Vector2f.h"
#include "imgui.h"
#include "physicsBody.h"
#include <cstdlib>

/**
 * @brief Creates background entity and sets its properties
 * @param setToBox The box object representing the background
 */
void createBackground(const Box &setToBox) {
  /** Pointer to the game entity representing the background */
  Entity *background = GameManager::createEntity("Background");

  /** Load the texture for the background entity */
  background->add<Sprite>()->loadTexture("img/background.png");

  /** Setting position of the background entity */
  background->get<entityBox>()->setPosition(setToBox.position);

  /** Setting size of the background entity */
  background->get<entityBox>()->setSize(setToBox.size);

  /** Specifies the layer of the background */
  background->layer = -10;
}

/**
 * @brief Creates a play button on the game canvas
 * @param canvas Pointer to the Entity that is the game canvas
 */
void createPlayButton(Entity *canvas) {
  /** Pointer to the game entity representing the UI */
  Entity *entity = GameManager::createEntity("ui");

  /** Set the parent of the UI entity */
  entity->setParent(canvas);

  /** Establish the entityBox for this entity */
  entityBox *box = entity->add<entityBox>();

  box->anchor = 4;
  box->setScale({500, 20 * 4});
  box->setLocalWithCenter({0, 300});

  /** Create a simple IMGUI panel within this entity */
  SimpleImGuiPanel *slice = entity->add<SimpleImGuiPanel>();

  /** Add Text component to the entity */
  // Text *text = entity->add<Text>();

  // text->changeFont("img/fonts/prstart.ttf", 20);
  // text->text = "Press Space to Play";

  /** Add StartMenuDetecter component to the entity */
  // entity->add<StartMenuDetecter>();
  
  slice->addToPanel = [](){
    if (ImGui::Button("Play")) {
      loadGameScene();
    }
  };
}

/**
 * @brief Creates an individual snowflake entity
 * @returns A pointer to the created snowflake entity
 */
Entity *createSnowflake() {
  /** Pointer to the game entity representing a snowflake */
  Entity *entity = GameManager::createEntity("Snowflake");

  entity->add<Sprite>()->loadTexture("img/Snowflake.png");
  entity->get<entityBox>()->setSize({3, 3});
  entity->layer = -1;

  /** Adds the Snowflake component to the entity */
  entity->add<Snowflake>();

  float randomXPos = randFloat(0, GameManager::gameWindowSize.x);
  entity->get<entityBox>()->setWithCenter({randomXPos, 0});

  return entity;
}

/**
 * @brief Creates a gift entity with specified properties
 * @param position The position of the gift entity
 * @param direction The direction of the gift entity
 * @param force The force of the gift
 * @param firedFrom Pointer to the entity that the gift is fired from
 * @returns A pointer to the created gift entity
 */
Entity *createGift(Vector2f position, Vector2f direction, float force,
                   Entity *firedFrom) {
  /** Pointer to the game entity representing the gift */
  Entity *entity = GameManager::createEntity("Gift");

  entity->get<entityBox>()->setSize({15, 15});
  entity->add<entityBox>()->setWithCenter(position);
  entity->add<Sprite>()->loadTexture("img/Gift.png", false);

  /** Add the Gravity component to the entity */
  entity->add<Gravity>();

  /** Set the velocity of the physical body of the entity */
  entity->get<physicsBody>()->velocity = direction * force;

  entity->add<Gift>()->firedFrom = firedFrom;

  return entity;
}

/**
 * @brief Creates a single portal entity
 * @param position The position of the portal
 * @returns A pointer to the created portal entity
 */
Entity *createSingePortal(Vector2f position) {
  /** Pointer to the game entity representing the portal */
  Entity *entity = GameManager::createEntity("SinglePortal");

  entity->add<Sprite>()->loadTexture("img/Portal1.png");
  entity->get<entityBox>()->setPosition(position);
  entity->get<entityBox>()->setSize({16, 16});
  entity->add<Portal>();

  /** Add animation to the sprite of the portal entity */
  entity->get<Sprite>()->addAnimation(
      "general", {"img/Portal1.png", "img/Portal2.png", "img/Portal3.png"},
      0.15);

  return entity;
}

/**
 * @brief Creates a pair of linked portal entities
 * @param APosition The position of the first portal
 * @param BPosition The position of the second portal
 * @returns A pointer to the entity representing the portal pair
 */
Entity *createPortal(Entity* entity, Vector2f APosition, Vector2f BPosition) {
  /** Pointer to the game entity representing the portal pair */
  // Entity *entity = GameManager::createEntity("Portal");

  /** Create two single portals */
  Entity *APortal = createSingePortal(APosition);
  Entity *BPortal = createSingePortal(BPosition);

  /** Link the portals */
  APortal->get<Portal>()->otherPortal = BPortal->get<Portal>();
  // APortal->setParent(entity);

  if (entity->checkComponent<MovePoint>()) {
    std::cout << "BOOOIES\n";
    APortal->add<MovePoint>();
    APortal->get<MovePoint>()->startPoint = entity->get<MovePoint>()->startPoint;
    APortal->get<MovePoint>()->endPoint = entity->get<MovePoint>()->endPoint;
    APortal->get<MovePoint>()->speed = entity->get<MovePoint>()->speed;
  }

  BPortal->get<Portal>()->otherPortal = APortal->get<Portal>();

  return entity;
}
