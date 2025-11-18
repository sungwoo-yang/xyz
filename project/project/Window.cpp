#include "Window.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Engine.h"  // Engine.GetWindow().Resize()

void Window::Initialize(const std::string& windowName) {
  mRenderWindow.create(sf::VideoMode(1280, 720), windowName,
                       sf::Style::Close | sf::Style::Resize);
}

void Window::Resize(int newWidth, int newHeight) {
  mScreenSize.x = newWidth;
  mScreenSize.y = newHeight;
}

Vector2DInt Window::GetSize() const { return mScreenSize; }

sf::RenderWindow& Window::GetWindow(void) { return mRenderWindow; }

void Window::Clear(sf::Color color) { mRenderWindow.clear(color); }

void Window::Draw(const sf::Drawable& drawable) {
  mRenderWindow.draw(drawable);
}

void Window::Update() { mRenderWindow.display(); }

void on_window_resized(int new_width, int new_height) {
  Engine::GetWindow().Resize(new_width, new_height);
}