#include "MainMenu.h"

#include <iostream>

#include "Engine.h"  //Engine::GetWindow()
#include "Levels.h"

MainMenu::MainMenu() {};

void MainMenu::Load() {
  if (!mFont.loadFromFile("../Assets/Font/04B_03__.TTF")) {
    std::cerr << "Failed to load font\n";
  }

  mMainMenuText.setFont(mFont);
  mMainMenuText.setString("Main Menu!\nPress Space");
  // mMainMenuText.setPosition(sf::Vector2f(0, 0));
  mMainMenuText.setCharacterSize(120);
  mMainMenuText.setStyle(sf::Text::Regular);

  sf::FloatRect textRect = mMainMenuText.getLocalBounds();
  mMainMenuText.setOrigin(textRect.left + textRect.width / 2.0f,
                          textRect.top + textRect.height / 2.0f);

  Vector2DInt windowSize = Engine::GetWindow().GetSize();
  mMainMenuText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);
}

void MainMenu::Draw() {
  Engine::GetWindow().Clear(sf::Color(51, 153, 218, 255));
  Engine::GetWindow().Draw(mMainMenuText);
}

void MainMenu::Update(double /* dt*/) {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
    Engine::GetGameStateManager().SetNextState(1);
  }
}

void MainMenu::Unload() {}