// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#pragma once
#include "FadeScreenState.h"
#include "GameStateManager.h"

template <typename NewState>
void FadeToState(std::unique_ptr<NewState>&& newState,
                 GameStateManager& stateManager, sf::Color color) {
  stateManager.PushState(std::make_unique<FadeScreenState>(
      Fade::In, color, 0.5,
      [&stateManager, color, state = newState.release()]() {
        stateManager.PopState();
        stateManager.PushState(std::unique_ptr<NewState>{state});
        stateManager.PushState(
            std::make_unique<FadeScreenState>(Fade::Out, color, 0.5));
      }));
}
