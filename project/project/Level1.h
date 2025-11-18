#pragma once
#include <SFML/Graphics.hpp>  //sf::Font, sf::Text

#include "GameState.h"  //class Level1 : public GameState (Inheritance)

class Level1 : public GameState {
 public:
  Level1();
  void Load() override;
  void Update(double dt) override;
  void Unload() override;
  void Draw() override;

  std::string GetName() override { return "Level1"; }

 private:
  sf::Font mFont;
  sf::Text mLevel1Text;
};