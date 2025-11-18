#include "Level1.h"

#include "Engine.h"  //Engine::GetWindow()

Level1::Level1(){};

void Level1::Load() {
  mFont.loadFromFile("../Assets/Font/04B_03__.ttf");
  mLevel1Text.setFont(mFont);
  mLevel1Text.setString("Level1");
  mLevel1Text.setPosition(sf::Vector2f(0, 0));
  mLevel1Text.setCharacterSize(30);
  mLevel1Text.setStyle(sf::Text::Regular);
}

void Level1::Draw() {
  Engine::GetWindow().Clear(sf::Color(100, 50, 50, 255));
  Engine::GetWindow().Draw(mLevel1Text);
}

void Level1::Update(double /* dt*/) {}

void Level1::Unload() {}