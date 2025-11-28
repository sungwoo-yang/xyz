// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <array>

namespace sf {
class Event;
}

class Window;

class Input {
 public:

  void HandleEvent(sf::Event inputEvent);

  [[nodiscard]] bool IsKeyPressed(sf::Keyboard::Key scancode) const;
  [[nodiscard]] bool IsKeyReleased(sf::Keyboard::Key scancode) const;

  [[nodiscard]] bool IsMousePressed(sf::Mouse::Button mouseButton) const;
  [[nodiscard]] bool IsMouseReleased(sf::Mouse::Button mouseButton) const;

  [[nodiscard]] sf::Vector2i GetMousePosition(void) const;

  void KeyUpdate();

 private:
  void SetPressedKey(sf::Keyboard::Key scancode);
  void SetReleasedKey(sf::Keyboard::Key scancode);

  void SetPressedMouse(sf::Mouse::Button mouseButton);
  void SetReleasedMouse(sf::Mouse::Button mouseButton);

  void SetMousePosition(sf::Vector2i position);

  void Reset();

 private:
  std::array<bool, sf::Keyboard::KeyCount> mKeyPressed{};
  std::array<bool, sf::Keyboard::KeyCount> mKeyReleased{};

  std::array<bool, sf::Mouse::ButtonCount> mMousePressed{};
  std::array<bool, sf::Mouse::ButtonCount> mMouseReleased{};

  sf::Vector2i mMouse{};
};
