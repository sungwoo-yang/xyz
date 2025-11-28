// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#pragma once
#include <gsl/pointers>

#include "GameState.h"

class Input;

class PlayState : public GameState {
 public:
  explicit PlayState(gsl::not_null<Input*> input) : mInput(input) {}

  void Enter() override {}
  void Exit() override {}
  void Update(double dt) override;
  void Draw(Window& window) override;
  std::string GetName() override { return "GameState"; }

 private:
  Input* mInput = nullptr;
};
