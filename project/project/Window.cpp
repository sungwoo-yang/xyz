// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#include "Window.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

void Window::Initialize(const std::string& windowName) {
  mRenderWindow.create(sf::VideoMode(1280, 720), windowName, sf::Style::Close);
}

sf::Vector2i Window::GetSize() const {
  const auto [w, h] = mRenderWindow.getSize();
  return {static_cast<int>(w), static_cast<int>(h)};
}

sf::RenderWindow& Window::GetWindow(void) { return mRenderWindow; }

void Window::Clear(sf::Color color) { mRenderWindow.clear(color); }

void Window::Draw(const sf::Drawable& drawable) {
  mRenderWindow.draw(drawable);
}

void Window::Update() { mRenderWindow.display(); }
