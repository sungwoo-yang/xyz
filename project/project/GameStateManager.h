#pragma once
// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#include <memory>
#include <vector>

#include "GameState.h"

class Window;

class GameStateManager {
 public:
  void PushState(std::unique_ptr<GameState>&& gameState);
  void PopState();
  void Update(double dt);
  void Draw(Window& window);
  [[nodiscard]] bool HasGameEnded() const { return mGameStateStack.empty(); }
  void Clear();

 private:
  std::vector<std::unique_ptr<GameState>> mGameStateStack;
  std::vector<std::unique_ptr<GameState>> mToClear;
};
