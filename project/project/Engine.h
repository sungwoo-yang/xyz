#pragma once

#include <chrono>  //std::chrono::system_clock::time_point
#include <string>  //std::string

#include "GameStateManager.h"  //GameStateManager
#include "Window.h"            //Window

class Engine {
 public:
  static Engine& Instance() {
    static Engine instance;
    return instance;
  }
  static Window& GetWindow() { return Instance().mWindow; }
  static GameStateManager& GetGameStateManager() {
    return Instance().mGameStateManager;
  }

  void Initialize(std::string windowName);
  void Shutdown();
  void Update();
  bool HasGameEnded();

 private:
  Engine();
  ~Engine();

  GameStateManager mGameStateManager;
  Window mWindow;

  std::chrono::system_clock::time_point mNow, mLastTick;
  static constexpr double TARGET_FPS = 30.0;
  double mFrameCount = 0;
  double mTimer = 0;
};