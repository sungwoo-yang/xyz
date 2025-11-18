#include "Window.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "Engine.h"  // Engine.GetWindow().Resize()

void Window::Initialize(const std::string& windowName) {
  mRenderWindow.create(sf::VideoMode(1280, 720), windowName,
                       sf::Style::Close | sf::Style::Resize);
  mScreenSize.x = 1280;
  mScreenSize.y = 720;
}

void Window::Resize(int newWidth, int newHeight) {
  mScreenSize.x = newWidth;
  mScreenSize.y = newHeight;
  sf::FloatRect visibleArea(0, 0, (float)newWidth, (float)newHeight);
  mRenderWindow.setView(sf::View(visibleArea));
}

Vector2DInt Window::GetSize() const { return mScreenSize; }

sf::RenderWindow& Window::GetWindow(void) { return mRenderWindow; }

void Window::Clear(sf::Color color) { mRenderWindow.clear(color); }

void Window::Draw(const sf::Drawable& drawable) {
  mRenderWindow.draw(drawable);
}

void Window::Update() { mRenderWindow.display(); }
