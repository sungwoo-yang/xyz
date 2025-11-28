// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#include "FadeScreenState.h"

#include <SFML/Graphics/RectangleShape.hpp>

#include "ColorHelper.h"
#include "GameStateManager.h"
#include "Interpolate.h"
#include "Window.h"

FadeScreenState::FadeScreenState(Fade fadeType, const sf::Color& color,
                                 double totalTime,
                                 std::function<void()>&& callback)
    : mColor(color),
      mTotalTime(totalTime),
      mFadeType(fadeType),
      mCallback(std::move(callback)) {}

void FadeScreenState::Enter() { mColor.a = (mFadeType == Fade::In) ? 0 : 255; }

void FadeScreenState::Update(double dt) {
  mCurrentTime += dt;
  const double fade_start = (mFadeType == Fade::In) ? 0.0 : 255.0;
  const double fade_end = (mFadeType == Fade::In) ? 255.0 : 0.0;
  mOpacity = math::map_linearly_and_clamp(mCurrentTime, 0.0, mTotalTime,
                                          fade_start, fade_end);
  mColor.a = to_byte(mOpacity);

  if (mCurrentTime >= mTotalTime) {
    StateManager().PopState();
    mCallback();
  }
}

void FadeScreenState::Draw(Window& window) {
  const auto [width, height] = window.GetSize();
  sf::RectangleShape rect{
      sf::Vector2f{static_cast<float>(width), static_cast<float>(height)}};
  rect.setFillColor(mColor);
  window.Draw(rect);
}
