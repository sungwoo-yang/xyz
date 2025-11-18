#include "Engine.h"

#include <SFML/Window/Event.hpp>
#include <utility>

#define LIMITED_TIME 5

Engine::Engine() : mGameStateManager(){};

Engine::~Engine() {}

void Engine::Initialize(std::string windowName) {
  mWindow.Initialize(std::move(windowName));
}

void Engine::Shutdown() { mGameStateManager.Shutdown(); }

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
      }
    }

    mWindow.Update();
    mGameStateManager.Update(dt);
  }

  if (mTimer >= LIMITED_TIME) {
    mTimer = 0;
    mFrameCount = 0;
  }
}

bool Engine::HasGameEnded() { return mGameStateManager.HasGameEnded(); }
