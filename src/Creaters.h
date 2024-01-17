#include "Charlie2D.h"
#include "Components.h"

void createBackground(const Box &setToBox) {
  Entity *background = GameManager::createEntity("Background");
  background->add<Sprite>()->loadTexture("img/background.png");
  background->get<entityBox>()->setPosition(setToBox.position);
  background->get<entityBox>()->setSize(setToBox.size);
  background->layer = -1;
}

void createPlayButton(Entity *canvas) {
  Entity *entity = GameManager::createEntity("ui");
  entity->setParent(canvas);

  entityBox *box = entity->add<entityBox>();
  box->anchor = 4;
  box->setScale({500, 20 * 4});
  box->setLocalWithCenter({0, 300});

  SimpleImGuiPanel *slice = entity->add<SimpleImGuiPanel>();

  Text *text = entity->add<Text>();
  text->changeFont("img/fonts/prstart.ttf", 20);
  text->text = "Press Space to Play";

  entity->add<StartMenuDetecter>();
}

Entity *createGift(Vector2f position, Vector2f direction, float force,
                   Entity *firedFrom) {
  Entity *entity = GameManager::createEntity("Gift");
  entity->get<entityBox>()->setSize({15, 15});
  entity->add<entityBox>()->setWithCenter(position);
  entity->add<Sprite>()->loadTexture("img/Gift.png", false);
  entity->add<Gravity>();
  entity->get<physicsBody>()->velocity = direction * force;
  entity->add<Gift>()->firedFrom = firedFrom;
  return entity;
}
