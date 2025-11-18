#pragma once
#include <SFML/Graphics.hpp>  //sf::Font, sf::Text

#include "GameState.h"  //class MainMenu : public GameState (Inheritance)

class MainMenu : public GameState {
 public:
  MainMenu();
  void Load() override;
  void Update(double dt) override;
  void Unload() override;
  void Draw() override;

  std::string GetName() override { return "MainMenu"; }

 private:
  sf::Font mFont;
  sf::Text mMainMenuText;
};