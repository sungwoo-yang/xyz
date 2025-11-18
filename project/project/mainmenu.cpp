#include "MainMenu.h"

#include "Engine.h"  //Engine::GetWindow()
#include "Levels.h"

MainMenu::MainMenu(){};

void MainMenu::Load() {
  mFont.loadFromFile("../Assets/Font/04B_03__.ttf");
  mMainMenuText.setFont(mFont);
  mMainMenuText.setString("Main Menu!");
  mMainMenuText.setPosition(sf::Vector2f(0, 0));
  mMainMenuText.setCharacterSize(120);
  mMainMenuText.setStyle(sf::Text::Regular);
}

void MainMenu::Draw() {
  Engine::GetWindow().Clear(sf::Color(51, 153, 218, 255));
  Engine::GetWindow().Draw(mMainMenuText);
}

void MainMenu::Update(double /* dt*/) {}

void MainMenu::Unload() {}