#pragma once
#include <vector>  //std::vector

#include "GameState.h"  //GameState* currGameState

class GameStateManager {
 public:
  GameStateManager();

  void AddGameState(GameState* gameState);
  void Update(double dt);
  void SetNextState(int initState);
  void Shutdown();
  void ReloadState();
  bool HasGameEnded() const { return mState == State::EXIT; }

 private:
  void SetStartState();
  void SetLoadState();
  void SetRunningState(double dt);
  void SetUnloadState();
  void SetShutDownState();

  enum class State {
    START,
    LOAD,
    RUNNING,
    UNLOAD,
    SHUTDOWN,
    EXIT,
  };

  std::vector<GameState*> mGameStates;
  State mState;
  GameState* mCurrentGameState;
  GameState* mNextGameState;
};
