#pragma once
#include "Charlie2D.h"
#include <sstream>

std::string floatToString(float value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

Entity *createUIPanel(Entity *canvas, Box setBox, std::string text,
                      std::string tag = "UIPanel") {
  Entity *entity = GameManager::createEntity(tag);
  entity->setParent(canvas);
  entityBox *box = entity->add<entityBox>();
  box->anchor = 4;
  box->setPosition(setBox.position);
  box->setSize(setBox.size);
  entity->add<UISliceRenderer>();
  entity->add<Text>()->changeFont("img/fonts/prstart.ttf", 30);
  entity->get<Text>()->text = text;
  return entity;
}
