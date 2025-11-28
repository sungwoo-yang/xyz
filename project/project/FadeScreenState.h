// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#pragma once
#include <SFML/Graphics/Color.hpp>
#include <functional>

#include "GameState.h"

enum class Fade { In, Out };

class FadeScreenState : public GameState {
 public:
  FadeScreenState(
      Fade fadeType, const sf::Color& color, double totalTime,
      std::function<void()>&& callback = [] {});

  void Enter() override;
  void Exit() override {}
  void Update(double dt) override;
  void Draw(Window& window) override;
  std::string GetName() override { return "FadeScreenState"; }

 private:
  sf::Color mColor;
  double mOpacity{0};
  double mCurrentTime{0};
  const double mTotalTime{0};
  const Fade mFadeType{Fade::In};

  std::function<void(void)> mCallback;
};
