// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Window {
 public:
  void Initialize(const std::string& windowName);
  void Update();
  sf::Vector2i GetSize() const;
  sf::RenderWindow& GetWindow(void);
  void Draw(const sf::Drawable& drawable);
  void Clear(sf::Color color);

 private:
  sf::RenderWindow mRenderWindow;
};
