#include "GameStateManager.h"

#include "Engine.h"

GameStateManager::GameStateManager()
    : mCurrentGameState(nullptr),
      mNextGameState(nullptr),
      mState(State::START) {}

void GameStateManager::AddGameState(GameState* gameState) {
  mGameStates.push_back(gameState);
}

GameStateManager::~GameStateManager() {
  for (GameState* gamestate : mGameStates) {
    delete gamestate;
  }
  mGameStates.clear();
}

void GameStateManager::Update(double dt) {
  switch (mState) {
    case GameStateManager::State::START:
      SetStartState();
      break;

    case GameStateManager::State::LOAD:
      SetLoadState();
      break;

    case GameStateManager::State::RUNNING:
      SetRunningState(dt);
      break;

    case GameStateManager::State::UNLOAD:
      SetUnloadState();
      break;

    case GameStateManager::State::SHUTDOWN:
      SetShutDownState();
      break;

    default:
      break;
  }
}

void GameStateManager::SetNextState(int initState) {
  mNextGameState = mGameStates[initState];
}

void GameStateManager::ReloadState() { mState = State::UNLOAD; }

void GameStateManager::Shutdown() {
  mState = State::UNLOAD;
  mNextGameState = nullptr;
}

void GameStateManager::SetStartState() {
  mNextGameState = mGameStates[0];
  mState = State::LOAD;
}

void GameStateManager::SetLoadState() {
  mCurrentGameState = mNextGameState;

  mCurrentGameState->Load();
  mState = State::RUNNING;
}

void GameStateManager::SetRunningState(double dt) {
  if (mCurrentGameState != mNextGameState) {
    mState = State::UNLOAD;
  }

  mCurrentGameState->Update(dt);

  mCurrentGameState->Draw();
}

void GameStateManager::SetUnloadState() {
  mCurrentGameState->Unload();
  if (mNextGameState == nullptr) {
    mState = State::SHUTDOWN;
  } else {
    mState = State::LOAD;
  }
}

void GameStateManager::SetShutDownState() { mState = State::EXIT; }