#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "BasicDataTypes.h"

class Window {
 public:
  void Initialize(const std::string& windowName);
  void Update();
  void Resize(int newWidth, int newHeight);
  Vector2DInt GetSize() const;
  sf::RenderWindow& GetWindow(void);
  void Draw(const sf::Drawable& drawable);
  void Clear(sf::Color color);

 private:
  Vector2DInt mScreenSize;
  sf::RenderWindow mRenderWindow;
};