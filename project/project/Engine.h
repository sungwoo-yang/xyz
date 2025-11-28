// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#pragma once

#include <chrono>
#include <string>

#include "GameStateManager.h"
#include "Input.h"
#include "Window.h"

class Engine {
 public:
  void Initialize(const std::string& windowName);
  void Shutdown();
  void Update();
  bool HasGameEnded() const;

 private:
  GameStateManager mGameStateManager;
  Window mWindow;
  Input mInput;

  std::chrono::system_clock::time_point mNow, mLastTick;
  static constexpr double TARGET_FPS = 30.0;
  double mFrameCount = 0;
  double mTimer = 0;
};
