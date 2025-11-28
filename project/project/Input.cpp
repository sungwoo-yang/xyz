// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#include "Input.h"

#include <SFML/Window/Event.hpp>

#include "Engine.h"

void Input::HandleEvent(sf::Event inputEvent) {
  switch (inputEvent.type) {
    case sf::Event::EventType::KeyPressed:
      SetPressedKey(inputEvent.key.code);
      break;
    case sf::Event::EventType::KeyReleased:
      SetReleasedKey(inputEvent.key.code);
      break;
    case sf::Event::EventType::MouseMoved:
      SetMousePosition({inputEvent.mouseMove.x, inputEvent.mouseMove.y});
      break;
    case sf::Event::EventType::MouseButtonPressed:
      SetPressedMouse(inputEvent.mouseButton.button);
      break;
    case sf::Event::EventType::MouseButtonReleased:
      SetReleasedMouse(inputEvent.mouseButton.button);
      break;
    default:
      break;
  }
}

void Input::KeyUpdate() {
  for (int i = 0; i < sf::Keyboard::KeyCount; ++i) {
    mKeyReleased[i] = mKeyPressed[i];
  }

  for (int i = 0; i < sf::Mouse::ButtonCount; ++i) {
    mMouseReleased[i] = mMousePressed[i];
  }
}

bool Input::IsKeyPressed(sf::Keyboard::Key scancode) const {
  return mKeyPressed[scancode];
}

bool Input::IsKeyReleased(sf::Keyboard::Key scancode) const {
  return mKeyReleased[scancode];
}

bool Input::IsMousePressed(sf::Mouse::Button mouseButton) const {
  return mMousePressed[mouseButton];
}

bool Input::IsMouseReleased(sf::Mouse::Button mouseButton) const {
  return mMouseReleased[mouseButton];
}

sf::Vector2i Input::GetMousePosition(void) const { return mMouse; }

void Input::SetPressedKey(sf::Keyboard::Key scancode) {
  if (!mKeyPressed[scancode]) {
    mKeyPressed[scancode] = true;
  }
}

void Input::SetReleasedKey(sf::Keyboard::Key scancode) {
  if (!mKeyReleased[scancode]) {
    mKeyReleased[scancode] = true;
  }

  mKeyPressed[scancode] = false;
}

void Input::SetPressedMouse(sf::Mouse::Button mouseButton) {
  if (!mMousePressed[mouseButton]) {
    mMousePressed[mouseButton] = true;
  }
}

void Input::SetReleasedMouse(sf::Mouse::Button mouseButton) {
  if (!mMouseReleased[mouseButton]) {
    mMouseReleased[mouseButton] = true;
  }

  mMousePressed[mouseButton] = false;
}

void Input::SetMousePosition(sf::Vector2i position) {
  mMouse.x = position.x;
  mMouse.y = position.y;
}

void Input::Reset() {
  for (int i = 0; i < sf::Keyboard::KeyCount; ++i) {
    mKeyPressed[i] = false;
    mKeyReleased[i] = false;
  }

  for (int i = 0; i < sf::Mouse::ButtonCount; ++i) {
    mMousePressed[i] = false;
    mMouseReleased[i] = false;
  }
}
