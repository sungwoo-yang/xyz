#include <iostream>

#include "Engine.h"    //Engine::Instance();
#include "Level1.h"    //Level1
#include "MainMenu.h"  //MainMenu

int main() try {
  Engine& engine = Engine::Instance();

  engine.Initialize("PROJECT");

  engine.GetGameStateManager().AddGameState(new MainMenu);
  engine.GetGameStateManager().AddGameState(new Level1);

  while (engine.HasGameEnded() == false) {
    engine.Update();
  }

  engine.Shutdown();

  return 0;
} catch (std::exception& e) {
  std::cerr << e.what();
  return -1;
}
