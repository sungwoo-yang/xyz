// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#include "Engine.h"

#include <SFML/Window/Event.hpp>

#include "MainMenu.h"

constexpr double LIMITED_TIME = 5;

void Engine::Initialize(const std::string& windowName) {
  mWindow.Initialize(windowName);

  mGameStateManager.PushState(std::make_unique<MainMenu>(&mInput));
}

void Engine::Shutdown() { mGameStateManager.Clear(); }

void Engine::Update() {
  mNow = std::chrono::system_clock::now();
  const double dt = std::chrono::duration<double>(mNow - mLastTick).count();
  sf::Event event{};

  if (dt >= (1 / Engine::TARGET_FPS)) {
    mLastTick = mNow;
    mTimer += dt;
    ++mFrameCount;

    while (mWindow.GetWindow().pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        Shutdown();
        return;
      }
      mInput.HandleEvent(event);
    }
    mGameStateManager.Update(dt);
    mInput.KeyUpdate();
    mGameStateManager.Draw(mWindow);
    mWindow.Update();
  }

  if (mTimer >= LIMITED_TIME) {
    mTimer = 0;
    mFrameCount = 0;
  }
}

bool Engine::HasGameEnded() const { return mGameStateManager.HasGameEnded(); }
