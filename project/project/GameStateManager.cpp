// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#include "GameStateManager.h"

#include <iostream>

#include "Engine.h"

void GameStateManager::PushState(std::unique_ptr<GameState>&& gameState) {
  auto* const state = gameState.get();
  mGameStateStack.push_back(std::move(gameState));
  state->mStateManager = this;
  std::clog << "Entering state " << state->GetName() << "\n";
  state->Enter();
}

void GameStateManager::PopState() {
  auto* const state = mGameStateStack.back().get();
  std::clog << "Exiting state " << state->GetName() << "\n";
  state->Exit();
  mToClear.push_back(std::move(mGameStateStack.back()));
  mGameStateStack.erase(mGameStateStack.end() - 1);
}

void GameStateManager::Update(double dt) {
  mToClear.clear();
  mGameStateStack.back()->Update(dt);
}

void GameStateManager::Draw(Window& window) {
  window.Clear(sf::Color(51, 153, 218, 255));
  for (auto& game_state : mGameStateStack) {
    game_state->Draw(window);
  }
}

void GameStateManager::Clear() { mGameStateStack.clear(); }
