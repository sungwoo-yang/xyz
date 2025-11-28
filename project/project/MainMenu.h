// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#pragma once
#include <SFML/Graphics.hpp>
#include <gsl/pointers>

#include "GameState.h"

class Window;
class Input;

class MainMenu : public GameState {
 public:
  MainMenu(gsl::not_null<Input*> input);

  void Enter() override;
  void Update(double dt) override;
  void Exit() override;
  void Draw(Window& window) override;

  std::string GetName() override { return "MainMenu"; }

 private:
  sf::Font mFont;
  sf::Text mMainMenuText;
  Input* mInput = nullptr;
};
