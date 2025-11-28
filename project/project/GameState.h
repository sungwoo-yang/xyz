// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#pragma once
#include <string>

class Window;
class GameStateManager;

class GameState {
 public:
  virtual void Enter() = 0;
  virtual void Exit() = 0;
  virtual void Update(double dt) = 0;
  virtual void Draw(Window& window) = 0;
  virtual std::string GetName() = 0;

  GameState() = default;
  virtual ~GameState() = default;
  GameState(const GameState& other) = default;
  GameState(GameState&& other) noexcept = default;
  GameState& operator=(const GameState& other) = default;
  GameState& operator=(GameState&& other) noexcept = default;

 protected:
  [[nodiscard]] GameStateManager& StateManager() const {
    return *mStateManager;
  }

 private:
  GameStateManager* mStateManager = nullptr;
  friend class GameStateManager;
};
