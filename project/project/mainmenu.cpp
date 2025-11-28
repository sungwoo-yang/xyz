// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#include "MainMenu.h"

#include "Engine.h"
#include "FadeScreenState.h"
#include "PlayState.h"
#include "StateTransitions.h"

MainMenu::MainMenu(gsl::not_null<Input*> input) : mInput(input) {}

void MainMenu::Enter() {
  mFont.loadFromFile("../Assets/Font/04B_03__.ttf");
  mMainMenuText.setFont(mFont);
  mMainMenuText.setString("Main Menu!\n\tPress space bar to progress...");
  mMainMenuText.setPosition(sf::Vector2f(0, 0));
  mMainMenuText.setCharacterSize(40);
  mMainMenuText.setStyle(sf::Text::Regular);
}

void MainMenu::Draw(Window& window) { window.Draw(mMainMenuText); }

void MainMenu::Update([[maybe_unused]] double dt) {
  if (mInput->IsKeyReleased(sf::Keyboard::Space)) {
    FadeToState(std::make_unique<PlayState>(mInput), StateManager(),
                sf::Color::White);
  }
}

void MainMenu::Exit() {}
