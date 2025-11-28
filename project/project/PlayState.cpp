// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#include "PlayState.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "ColorHelper.h"
#include "FadeScreenState.h"
#include "GameStateManager.h"
#include "Input.h"
#include "MainMenu.h"
#include "StateTransitions.h"
#include "Window.h"

void PlayState::Update([[maybe_unused]] double dt) {
  if (mInput->IsKeyReleased(sf::Keyboard::Space)) {
    FadeToState(std::make_unique<MainMenu>(mInput), StateManager(),
                sf::Color::Red);
  }
}

void recursive_draw_box(Window& window, float cx, float cy, float w, float h) {
  sf::RectangleShape rect{{w, h}};
  rect.setFillColor(sf::Color{0});
  rect.setPosition(cx - w / 2, cy - h / 2);
  rect.setOutlineThickness(0.1f * w);
  const auto color = to_byte(w);
  rect.setOutlineColor(sf::Color{color, color, color, 255});
  window.Draw(rect);
  if (w < 20) return;
  recursive_draw_box(window, cx - w / 2, cy - h / 2, w / 2, h / 2);
  recursive_draw_box(window, cx + w / 2, cy - h / 2, w / 2, h / 2);
  recursive_draw_box(window, cx - w / 2, cy + h / 2, w / 2, h / 2);
  recursive_draw_box(window, cx + w / 2, cy + h / 2, w / 2, h / 2);
}

void PlayState::Draw(Window& window) {
  constexpr float player_radius{80};
  sf::CircleShape player{player_radius};
  const auto [x, y] = mInput->GetMousePosition();

  player.setPosition(x - player_radius, y - player_radius);

  const auto [width, height] = window.GetSize();
  recursive_draw_box(window, width * 0.5f, height * 0.5f, width * 0.5f,
                     height * 0.5f);

  window.Draw(player);
}
