// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020

#include <iostream>

#include "Engine.h"

int main() try {
  Engine engine;

  engine.Initialize("PROJECT");

  while (!engine.HasGameEnded()) {
    engine.Update();
  }

  return 0;
} catch (std::exception& e) {
  std::cerr << e.what();
  return -1;
}
